#include "dir.h"
#include <direct.h>
#include <list>
#include "str.h"
using namespace std;

	/* is directory
	**/
	bool isDir(const string& sPath){

#ifdef _WIN32
		bool bIsDir = false;
		char szCurDir[256] = { 0 };

		_getcwd(szCurDir, sizeof(szCurDir) - 1);
		if (0 == _chdir(sPath.c_str()))
		{
			bIsDir = true;
			_chdir(szCurDir);
		}

		return bIsDir;
#else //_WIN32
		struct stat64 statBuf;

		return ((0 == stat64(sPath.c_str(), &statBuf)) && S_ISDIR(statBuf.st_mode));
#endif //_WIN32
	}

	/* make directory
	**/
	int makeDir(const string& sPath){

#ifdef _WIN32
		string sDir;
		int i, nSize = (int)sPath.size();
		SECURITY_ATTRIBUTES SecurityAttri = { sizeof(SECURITY_ATTRIBUTES), NULL, FALSE };

		for (i = 0; i <= nSize; i++){

			/* find node */
			if ((i == nSize) || ('/' == sPath[i]) || ('\\' == sPath[i])){

				/* check dir */
				sDir = sPath.substr(0, i);
				if (isDir(sDir)){
					continue;
				}

				/* create dir */
				if (TRUE != ::CreateDirectoryA(sDir.c_str(), &SecurityAttri)){
					return -1;
				}
			}
		}
#else //_WIN32
		/* check params */
		if (sPath.length() <= 0){
			return -1;
		}

		/* make dir if not dir */
		if (!isDir(sPath)){
			int i, nLen = sPath.length();

			for (i = 0; i < nLen; i++){
				if (('/' == sPath[i]) || (i == nLen - 1)){
					string sDir = sPath.substr(0, i + 1);
					if ((0 != mkdir(sDir.c_str(), 0755)) && (EEXIST != errno)){
						printf("mkdir %s failed(%d,%s)\n", sDir.c_str(), errno, strerror(errno));
						return -2;
					}
				}
			}
		}
#endif //_WIN32

		return 0;
	}

/* list dir
 */
int listDir(const string& sDir,int nRecursive,ONFINDFILE pfOnFindFile,void *lpParameter)
{
	HANDLE hFile = NULL;
	WIN32_FIND_DATAA wfd;	
	char szFileName[MAX_PATH] = {0};
	char szCurrentDir[MAX_PATH] = {0};
	
	/* check params */
	if (NULL == pfOnFindFile)
	{
		return -1;
	}
	
	/* get current directory */		
	_getcwd (szCurrentDir, MAX_PATH);
	
	/* change to search dir */
	_chdir (sDir.c_str ());
	
	/* find first file */
	sprintf_s(szFileName, sizeof(szFileName) - 1, "%s\\*", sDir.c_str());
	if(INVALID_HANDLE_VALUE == (hFile = FindFirstFileA (szFileName, &wfd))) 
	{
		_chdir (szCurrentDir);
		return -2;
	}		
	
	/* find next file */
	do 
	{
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if ('.' != wfd.cFileName[0])
			{
				if (nRecursive)
				{
					char szSubDir[MAX_PATH] = {0};
					
					sprintf_s(szSubDir, sizeof(szSubDir) - 1, "%s\\%s", sDir.c_str(), wfd.cFileName);
					listDir (szSubDir, nRecursive, pfOnFindFile, lpParameter);					
				}
				pfOnFindFile (sDir, wfd, lpParameter);
			}
		}
		else
		{
			pfOnFindFile (sDir, wfd, lpParameter);
		}	
	}while (FindNextFileA (hFile, &wfd));
	
	/* find close */
	FindClose (hFile);
	
	/* change to current dir */
	_chdir (szCurrentDir);
	
	return 0;
}

/* remove dir
 */
bool removeDir(const string& sDir)
{
	string sPath;
	HANDLE hFind;	
	WIN32_FIND_DATAA wfd;

	/* check params */
	if (sDir.length () <= 0)
	{
		return false;
	}
	
	/* find first file */
	if (INVALID_HANDLE_VALUE == (hFind = FindFirstFileA((sDir + "\\*.*").c_str(), &wfd)))
	{
		return false;
	}
	
	/* find next file */
	do
	{
		sPath = sDir + "\\" + wfd.cFileName;
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if ('.' != wfd.cFileName[0])
			{
				if (!removeDir (sPath))
				{
					return false;
				}
			}				
		}
		else
		{
			if (!DeleteFileA (sPath.c_str()))
			{
				return false;
			}
		}
	}
	while (FindNextFileA (hFind, &wfd));

	/* find close */
	FindClose(hFind);

	/* delete dir */
	if (!RemoveDirectoryA (sDir.c_str()))
	{
		return false;
	}

	return true;
}
/*Get a system temp directory*/
string getSystemTempDir()
{
	int nLen;
	char szPath[4096] = {0};

	/* get temp dir */
	if ((nLen = GetTempPathA(4096, szPath)) <= 0)
	{
		return string();
	}

	/* format path */
	if ('\\' == szPath[nLen-1])
	{
		szPath[nLen-1] = '\0';
	}

	return szPath;
}
/* get drive info
 */
int getDriveInfo(list<DRIVE_INFO_T> *plistDriveInfos)
{
	int nLen;
	char *p;
	char szDrives[256] = {0};
	
	/* check params */
	if (NULL == plistDriveInfos)
	{
		return -1;
	}
	plistDriveInfos->clear();
	
	/* get driver string */
	if ((0 == (nLen = GetLogicalDriveStringsA (sizeof (szDrives), szDrives))) || (nLen > sizeof (szDrives)))
	{
		return -2;
	}
	
	for(p = szDrives; *p; p += 4)
	{
		DRIVE_INFO_T driveInfo;
		char szVolumeName[256] = {0};
		char szFileSystem[256] = {0};
		DWORD dwSerialNumber,dwMaxComponentLength,dwFileSystem;
		ULARGE_INTEGER liFreeBytesAvailableToCaller;
		ULARGE_INTEGER liTotalNumberOfBytes;
		ULARGE_INTEGER liTotalNumberOfFreeBytes;
		
		/* root path */
		driveInfo.sRootPath = p;
		
		/* drive type */
		driveInfo.uType = GetDriveTypeA(p);
		
		/* get volume info */
		if (GetVolumeInformationA (p, szVolumeName, sizeof(szVolumeName), &dwSerialNumber, 
			&dwMaxComponentLength, &dwFileSystem, szFileSystem, sizeof(szFileSystem)))
		{
			driveInfo.sVolumeName = szVolumeName;
			driveInfo.dwSerialNumber = dwSerialNumber;
			driveInfo.dwMaxComponentLength = dwMaxComponentLength;
			driveInfo.dwFileSystem = dwFileSystem;
			driveInfo.sFileSystem = szFileSystem;
		}
		
		/* get space info */
		if (GetDiskFreeSpaceExA (p, 
			&liFreeBytesAvailableToCaller,
			&liTotalNumberOfBytes, 
			&liTotalNumberOfFreeBytes))
		{
			driveInfo.llTotalSpace = liTotalNumberOfBytes.QuadPart;
			driveInfo.llFreeSpace = liTotalNumberOfFreeBytes.QuadPart;
		}
		
		/* save drive info */
		plistDriveInfos->push_back(driveInfo);
	}
	
	return (int)plistDriveInfos->size();
}

/* list drives
 */
int listDrives(vector<DRIVE_INFO_T> *pvecDriveInfos)
{
	list<DRIVE_INFO_T> listDriveInfos;
	list<DRIVE_INFO_T>::iterator it;

	/* check params */
	if (NULL == pvecDriveInfos)
	{
		return -1;
	}
	pvecDriveInfos->clear ();

	/* get drive it's sapce > 0 */
	if (getDriveInfo(&listDriveInfos) > 0)
	{		
		for(it=listDriveInfos.begin(); it!=listDriveInfos.end(); ++it)
		{
			if (it->llTotalSpace > 0){
				pvecDriveInfos->push_back(*it);
			}			
		}
	}

	return (int)pvecDriveInfos->size ();
}

/* string drive type
 */
string stringDriveType(unsigned int uType)
{
	struct
	{
		unsigned int uType;
		char szName[256];
	} typeArray[] = {
		{ DRIVE_UNKNOWN,     "UNKNOWN-DISK" },
		{ DRIVE_NO_ROOT_DIR, "INVALID-DISK" },
		{ DRIVE_REMOVABLE,   "REMOVABLE-DISK" },
		{ DRIVE_FIXED,       "LOCAL-DISK" },
		{ DRIVE_REMOTE,      "NETWORK-DISK" },
		{ DRIVE_CDROM,       "CD-ROM" },
		{ DRIVE_RAMDISK,     "RAM-DISK"}
	};
	int i,nSize = sizeof(typeArray) / sizeof(typeArray[0]);

	for (i=0; i<nSize; i++)
	{
		if (uType == typeArray[i].uType)
		{
			return typeArray[i].szName;
		}
	}

	return string();
}
