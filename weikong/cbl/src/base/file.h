#if !defined(FILE_H__20190129)
#define FILE_H__20190129

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <list>

using namespace std;

namespace cbl {
	
//按行遍历读文件时的回调函数
typedef int (*PFONNEWLINE)(const char *szLine, void *parameter);

//读取文件，按行存到list，成功返回行数，失败返回负数
int readTextFile(const string &sFileName, list<string> *pvecLine);

//读取文件，按行存到list，成功返回行数，失败返回负数
int readTextFile(FILE *fp, list<string> *pvecLine);

//读取文件，按行调用回调函数
int readTextFile(const string &sFileName, PFONNEWLINE pfOnNewLine, void *parameter);

//读取文件，按行调用回调函数
int readTextFile(FILE *fp, PFONNEWLINE pfOnNewLine, void *parameter);

//按指定的读写模式打开文件，并定位到指定的位置
int locateFile(const string &sFilePath, bool bEnableRead, bool bEnableWrite, int nOffset, int nWhence, int &fd);

//把指定长度的内容写入到fd
int writeToFile(const void *pBuffer, size_t nSize, int fd);

//把指定长度的内容写入到fd
int writeToFile(const string &sData, int fd);

//获取文件大小
long long getFileSize(int fd);

//获取文件大小
long long getFileSize(const string &sFilePath);

//从文件的指定位置读取一定大小的内容到buf中，成功返回读取的大小，调用者要负责释放ppBuffer，失败返回负数
int loadFromFile(const string &sFilePath, int nOffset, int nWhence, int nSize, unsigned char **ppBuffer, int nLimitSize = (16 << 20));

//从文件的指定位置读取一定大小的内容到string中，成功返回读取的大小，失败返回负数
int loadFromFile(const string &sFilePath, int nOffset, int nWhence, int nSize, string &sData, int nLimitSize = (16 << 20));

//读取文件所有内容到buf中，成功返回读取的大小，调用者要负责释放ppBuffer，失败返回负数
int loadFromFile(const string &sFilePath, unsigned char **ppBuffer, int nLimitSize = (16 << 20));

//读取文件所有内容到string中，，成功返回读取的大小，失败返回负数
int loadFromFile(const string &sFilePath, string &sData, int nLimitSize = (16 << 20));

//把buf中的内容写到指定的文件
int saveToFile(const void *pBuffer, int nSize, const string &sFilePath);

//把string中的内容写入到指定的文件
int saveToFile(const string &sData, const string &sFilePath);

} 

#endif 
