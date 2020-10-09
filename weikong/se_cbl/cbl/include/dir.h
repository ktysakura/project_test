#if !defined(DIR_H__20180903)
#define DIR_H__20180903


#include <string>
#include <map>
#include <list>
#include <vector>
#include <windows.h>
using namespace std;

	/* define drive info
	*/
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

		/* construct */
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
	}DRIVE_INFO_T;

	/* define find file callback
	*/
	typedef int(*ONFINDFILE)(const string& sDir, const WIN32_FIND_DATAA& wfd, void *lpParameter);

	/* function declare
	*/
	bool isDir(const string& sPath);
	int makeDir(const string& sPath);
	int listDir(const string& sDir, int nRecursive, ONFINDFILE pfOnFindFile, void *lpParameter);
	bool removeDir(const string& sDir);
	string getSystemTempDir();
	int getDriveInfo(list<DRIVE_INFO_T> *plistDriveInfos);
	int listDrives(vector<DRIVE_INFO_T> *pvecDriveInfos);
	string stringDriveType(unsigned int uType);
	
#endif //DIR_H__20180903
