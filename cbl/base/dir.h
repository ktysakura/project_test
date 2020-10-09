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

//����Ŀ¼ʱ�Ļص�����ԭ��
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
 * ���ܣ�����Ŀ¼���ҵ��Ľ�������ص��������������ַ�����������utf8�汾
 * 
 * ������
 *	sDir			��Ҫ��������Ŀ¼����
 *	bRecursive		�Ƿ�ݹ����
 *	pfOnFindFile	�ص���������ѱ��������ļ�WIN32_FIND_DATA_UTF8�ṹ����ȥ
 *	parameter		�ص������Ĳ���
 *
 * ���أ��ɹ�����0��ʧ�ܷ��ظ���
 */
int listDir(const string &sDir, bool bRecursive, PFONFINDFILE pfOnFindFile, void *parameter);

/*
 * ���ܣ���ȡϵͳ���з�����Ϣ
 *
 * ���أ��ɹ�����0��ʧ�ܷ��ظ���
 */
int getDriveInfo(list<DRIVE_INFO_T> *plistDriveInfos);

//�������ͻ�ȡ�������͵��ַ�����ʽ
string stringDriveType(unsigned int uType);

//��ȡ��ʱĿ¼�ַ���
string getTempPathString();

//��ȡ��������Ŀ¼�ַ���
string getModulePathString();

//�жϸ������ַ����Ƿ���Ŀ¼
bool isDir(const string &sPath);

//�жϸ������ļ���Ŀ¼�Ƿ����
bool isPathFileExists(const string &sPath);

//�ݹ鴴��Ŀ¼
bool makeDir(const string &sPath);	

//ɾ���ƶ�Ŀ¼
bool removeDir(const string &sDir);

} 

#endif
