#include "LogFile.h"
#include <time.h>
#include <stdarg.h>
#include "AutoLock.h"
#include "dir.h"
#include "str.h"

namespace cbl{
/* init variable
 */
CLogFile *CLogFile::_instance = NULL;

/* get instance
 */
CLogFile *CLogFile::getInstance(){
	if (NULL == _instance){
		_instance = new CLogFile();
	}

	return _instance;
}

/* del instance
 */
int CLogFile::delInstance(){
	if (_instance){
		delete _instance;
		_instance = NULL;
	}

	return 0;
}

/* construct
 */
CLogFile::CLogFile()
{
	this->m_sDir = "./";
}

/* destruct
 */
CLogFile::~CLogFile()
{
}

/* set file
 */
int CLogFile::setLogDir(const string& sDir)
{
	CAutoLock locker(&m_mutex);

	/* make dir */
	if ((!isDir(sDir)) && (!makeDir(sDir))){
		return -1;
	}
	this->m_sDir = formatPath(sDir);

	return 0;
}

/* log
 */
int CLogFile::log(LOG_LEVEL_T level, const char *format, ...){
	va_list vl;

	va_start(vl, format);
	this->log(level, format, vl);
	va_end(vl);

	return 0;
}

/* log info
 */
int CLogFile::logi(const char *format, ...)
{
	va_list vl;

	va_start(vl, format);
	this->log(LOG_LEVEL_INFO, format, vl);
	va_end(vl);

	return 0;
}

/* log warning
 */
int CLogFile::logw(const char *format, ...)
{
	va_list vl;

	va_start(vl, format);
	this->log(LOG_LEVEL_WARNING, format, vl);
	va_end(vl);

	return 0;
}

/* log error
 */
int CLogFile::loge(const char *format, ...)
{
	va_list vl;

	va_start(vl, format);
	this->log(LOG_LEVEL_ERROR, format, vl);
	va_end(vl);

	return 0;
}

/* log
 */
int CLogFile::log(LOG_LEVEL_T level, const char *format, va_list& vl){
	FILE* fp = NULL;
	string sFileName, sFilePath;
	char szLogInfo[4096] = { 0 };
	time_t timeNow = time(NULL);
	struct tm *ptm = localtime(&timeNow);
	
	/* check file path */
	if ((m_sDir.length() <= 0) || (NULL == format) || (strlen(format) <= 0)){
		return -1;
	}

	/* get file name */
	switch (level){
	case LOG_LEVEL_INFO:{
		sFileName = "info.log";
		break;
	}
	case LOG_LEVEL_WARNING:{
		sFileName = "warning.log";
		break;
	}
	case LOG_LEVEL_ERROR:{
		sFileName = "error.log";
		break;
	}
	default:{
		return -2;
	}
	}

	CAutoLock locker(&m_mutex);
	/* open file */
	if (NULL == (fp = fopen((m_sDir + "/" + sFileName).c_str(), "a+")))
	{
		return -3;
	}

	/* print log info */
	vsprintf(szLogInfo, format, vl);
	fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
		ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, szLogInfo);

	/* close file */
	fclose(fp);

	return 0;
}

}