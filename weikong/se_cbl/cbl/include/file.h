#if !defined(FILE_H__2019_11_13)
#define FILE_H__2019_11_13

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "encode.h"
using namespace std;
namespace cbl{
class  CFile
{
public:
	CFile();
	explicit CFile(const char* szFilePath);
	explicit CFile(const string& sFilePath);
	virtual ~CFile();
	int open(const char* szFileName, const char * szMode);
	int open(const string& sFilePath, const char * szMode);
	bool  isExists() const;
	size_t getFileSize();
	static long long getFileCTime(const string& sFilePath);
	static long long getFileSize(const string& sFilePath);
	size_t read(void* data, size_t size);
	size_t read(string& sData, size_t size);
	size_t write(const void* pBuffer, size_t nBufferSize);
	size_t write(string& sData);
	size_t write(void *data, size_t size, int nWhence, size_t nOffset);
	size_t seek(size_t nOffset, size_t nWhence);
	size_t truncate(size_t size);
	size_t truncate();
	int flush();
	int close();
	bool isOpen();
public:
//	static int encodeName();
	static int rename(const string &oldName, const string &newName);
	static int remove(const string &fileName);
	static bool isExists(const string &fileName);
private:
	void init();
	FILE *m_fp;
	bool m_bOpened;
	string m_sFilePath;
};


}

#endif