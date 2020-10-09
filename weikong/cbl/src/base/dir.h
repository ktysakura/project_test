#if !defined(DIR_H__20180903)
#define DIR_H__20180903

#include <string>
#include <list>
#include <vector>
#include <windows.h>

using namespace std;

namespace cbl {

typedef struct _WIN32_FIND_DATA_UTF8 {  
	DWORD dwFileAttributes;  
	FILETIME ftCreationTime;  
	FILETIME ftLastAccessTime;  
	FILETIME ftLastWriteTime;  
	DWORD nFileSizeHigh;  
	DWORD nFileSizeLow;  
	DWORD dwReserved0;  
	DWORD dwReserved1;  
	string cFileName;
	string cAlternateFileName;
} WIN32_FIND_DATA_UTF8;

//遍历目录时的回调函数原型
typedef int (*PFONFINDFILE)(const string &sDir, const WIN32_FIND_DATA_UTF8 &wfd, void *parameter);

typedef struct tagDRIVE_INFO_T 
{
	string sRootPath;
	string sVolumeName;
	string sFileSystem;
	unsigned int uType;
	DWORD dwSerialNumber;
	DWORD dwMaxComponentLength;
	DWORD dwFileSystem;
	__int64 llTotalSpace;
	__int64 llFreeSpace;

	tagDRIVE_INFO_T()
	{
		sRootPath = "";
		sVolumeName = "";
		sFileSystem = "";
		uType = 0;
		dwSerialNumber = 0;
		dwMaxComponentLength = 0;
		dwFileSystem = 0;
		llTotalSpace = 0;
		llFreeSpace = 0;
	}
} DRIVE_INFO_T;

/*
 * 功能：遍历目录，找到的结果传给回调函数处理，所有字符串参数都是utf8版本
 * 
 * 参数：
 *	sDir			需要被遍历的目录名称
 *	bRecursive		是否递归遍历
 *	pfOnFindFile	回调函数，会把遍历到的文件WIN32_FIND_DATA_UTF8结构传进去
 *	parameter		回调函数的参数
 *
 * 返回：成功返回0，失败返回负数
 */
int listDir(const string &sDir, bool bRecursive, PFONFINDFILE pfOnFindFile, void *parameter);

/*
 * 功能：获取系统所有分区信息
 *
 * 返回：成功返回0，失败返回负数
 */
int getDriveInfo(list<DRIVE_INFO_T> *plistDriveInfos);

//根据类型获取分区类型的字符串形式
string stringDriveType(unsigned int uType);

//获取临时目录字符串
string getTempPathString();

//获取进程所在目录字符串
string getModulePathString();

//判断给定的字符串是否是目录
bool isDir(const string &sPath);

//判断给定的文件或目录是否存在
bool isPathFileExists(const string &sPath);

//递归创建目录
bool makeDir(const string &sPath);	

//删除制定目录
bool removeDir(const string &sDir);

} 

#endif
