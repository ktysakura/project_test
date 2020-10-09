#include "dir.h"
#include "str.h"
#include "encode.h"
#include "DataType.h"
#include <shlwapi.h>
#include <direct.h>

#pragma comment(lib, "shlwapi.lib")

namespace cbl {

//把宽字符版本的WIN32_FIND_DATA转成utf8版本
static void convertFindData(const WIN32_FIND_DATAW &w, WIN32_FIND_DATA_UTF8 &u)
{
	u.dwFileAttributes	= w.dwFileAttributes;
	u.ftCreationTime	= w.ftCreationTime;
	u.ftLastAccessTime	= w.ftLastAccessTime;
	u.ftLastWriteTime	= w.ftLastWriteTime;
	u.nFileSizeHigh		= w.nFileSizeHigh;
	u.nFileSizeLow		= w.nFileSizeLow;
	u.dwReserved0		= w.dwReserved0;
	u.dwReserved1		= w.dwReserved1;

	u.cFileName = utf16_to_utf8(w.cFileName);
	u.cAlternateFileName = utf16_to_utf8(w.cAlternateFileName);
}

int listDir(const string &sDir, bool bRecursive, PFONFINDFILE pfOnFindFile, void *parameter)
{
	HANDLE hFile = NULL;
	WIN32_FIND_DATAW wfd_w;
	WIN32_FIND_DATA_UTF8 wfd_u;
	wchar_t szFileName[MAX_PATH] = {0};
	wchar_t szCurrentDir[MAX_PATH] = {0};
	wstring sDir_w;
	
	if (sDir.empty() || NULL == pfOnFindFile) {
		return -1;
	}
	
	sDir_w = utf8_to_utf16(sDir);
	_wgetcwd(szCurrentDir, MAX_PATH);
	_wchdir(sDir_w.c_str());
	
	swprintf_s(szFileName, MAX_PATH - 1, L"%ls\\*", sDir_w.c_str());
	if (INVALID_HANDLE_VALUE == (hFile = FindFirstFileW(szFileName, &wfd_w))) {
		_wchdir(szCurrentDir);
		return -2;
	}		
	
	do {
		if (wfd_w.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (L'.' != wfd_w.cFileName[0]) {
				if (bRecursive) {
					wchar_t szSubDir[MAX_PATH] = {0};
					
					swprintf_s(szSubDir, MAX_PATH - 1, L"%ls\\%ls", sDir_w.c_str(), wfd_w.cFileName);
					listDir(utf16_to_utf8(szSubDir), bRecursive, pfOnFindFile, parameter);					
				}
				convertFindData(wfd_w, wfd_u);
				pfOnFindFile(sDir, wfd_u, parameter);
			}
		} else {
			convertFindData(wfd_w, wfd_u);
			pfOnFindFile(sDir, wfd_u, parameter);
		}	
	} while (FindNextFileW(hFile, &wfd_w));
	
	FindClose(hFile);
	_wchdir(szCurrentDir);
	
	return 0;
}

int getDriveInfo(list<DRIVE_INFO_T> *plistDriveInfos)
{
	int nLen;
	wchar_t szDrives[256] = {0};
	wchar_t *p;
	
	if (NULL == plistDriveInfos) {
		return -1;
	}

	plistDriveInfos->clear();
	
	//获取所有分区名称
	if ((0 == (nLen = GetLogicalDriveStringsW(ARRAY_LEN(szDrives), szDrives))) || (nLen > ARRAY_LEN(szDrives))) {
		return -2;
	}
	
	for (p = szDrives; *p; p += wcslen(p) + 1) {
		DRIVE_INFO_T driveInfo;
		wchar_t szVolumeName[256] = {0};
		wchar_t szFileSystem[256] = {0};
		DWORD dwSerialNumber, dwMaxComponentLength, dwFileSystem;
		ULARGE_INTEGER liFreeBytesAvailableToCaller;
		ULARGE_INTEGER liTotalNumberOfBytes;
		ULARGE_INTEGER liTotalNumberOfFreeBytes;
		
		//根路径
		driveInfo.sRootPath = utf16_to_utf8(p);

		//分区类型
		driveInfo.uType = GetDriveTypeW(p);
		
		//获取卷标信息
		if (GetVolumeInformationW(p, szVolumeName, ARRAY_LEN(szVolumeName), &dwSerialNumber, 
				&dwMaxComponentLength, &dwFileSystem, szFileSystem, ARRAY_LEN(szFileSystem))) 
		{
			driveInfo.sVolumeName = utf16_to_utf8(szVolumeName);
			driveInfo.dwSerialNumber = dwSerialNumber;
			driveInfo.dwMaxComponentLength = dwMaxComponentLength;
			driveInfo.dwFileSystem = dwFileSystem;
			driveInfo.sFileSystem = utf16_to_utf8(szFileSystem);
		}
		
		//获取分区空间信息
		if (GetDiskFreeSpaceExW(p, 
				&liFreeBytesAvailableToCaller,
				&liTotalNumberOfBytes, 
				&liTotalNumberOfFreeBytes)) 
		{
			driveInfo.llTotalSpace = liTotalNumberOfBytes.QuadPart;
			driveInfo.llFreeSpace = liTotalNumberOfFreeBytes.QuadPart;
		}
		
		plistDriveInfos->push_back(driveInfo);
	}
	
	return 0;
}

string stringDriveType(unsigned int uType)
{
	struct DriveType
	{
		unsigned int uType;
		char szName[256];
	};

	DriveType typeArray[] = {
		{DRIVE_UNKNOWN,     "UNKNOWN-DISK"},
		{DRIVE_NO_ROOT_DIR, "INVALID-DISK"},
		{DRIVE_REMOVABLE,   "REMOVABLE-DISK"},
		{DRIVE_FIXED,       "LOCAL-DISK"},
		{DRIVE_REMOTE,      "NETWORK-DISK"},
		{DRIVE_CDROM,       "CD-ROM"},
		{DRIVE_RAMDISK,     "RAM-DISK"}
	};

	for (int i = 0; i < ARRAY_LEN(typeArray); i++) {
		if (uType == typeArray[i].uType) {
			return typeArray[i].szName;
		}
	}

	return string();
}

string getTempPathString()
{
	int nLen;
	wchar_t szPath[4096] = {0};

	if ((nLen = GetTempPathW(ARRAY_LEN(szPath), szPath)) <= 0) {
		return string();
	}

	if (L'\\' == szPath[nLen - 1]) {
		szPath[nLen - 1] = L'\0';
	}

	return utf16_to_utf8(szPath);
}

string getModulePathString()
{
	int nLen;
	wchar_t szPath[4096] = {0};

	if ((nLen = GetModuleFileNameW(NULL, szPath, ARRAY_LEN(szPath) - 1)) <= 0) {
		return string();
	}

	if (L'\\' == szPath[nLen - 1]) {
		szPath[nLen - 1] = L'\0';
	}

	return utf16_to_utf8(szPath);
}

bool isDir(const string &sPath)
{
	BOOL ret;
	wstring sPath_w;

	if (sPath.empty()) {
		return true;
	}

	sPath_w = utf8_to_utf16(sPath);
	ret = PathIsDirectoryW(sPath_w.c_str());

	return (ret != FALSE);
}

bool isPathFileExists(const string &sPath)
{
	BOOL ret;
	wstring sPath_w;

	if (sPath.empty()) {
		return true;
	}

	sPath_w = utf8_to_utf16(sPath);
	ret = PathFileExistsW(sPath_w.c_str());

	return (ret != FALSE);
}

bool makeDir(const string &sPath)
{
	if (sPath.empty()) {
		return false;
	}
	
	wstring sPath_w;
	wstring sDir;
	
	sPath_w = utf8_to_utf16(sPath);
	int nSize = (int)sPath_w.size();
	SECURITY_ATTRIBUTES SecurityAttri = { sizeof(SECURITY_ATTRIBUTES), NULL, FALSE };

	for (int i = 0; i <= nSize; i++) {
		if ((i == nSize) || (L'/' == sPath_w[i]) || (L'\\' == sPath_w[i])) {
			sDir = sPath_w.substr(0, i);
			if (isDir(utf16_to_utf8(sDir))) {
				continue;
			}

			if (TRUE != CreateDirectoryW(sDir.c_str(), &SecurityAttri)) {
				return false;
			}
		}
	}

	return true;
}

bool removeDir(const string &sDir)
{
	wstring sDir_w;
	wstring sPath;
	HANDLE hFind;	
	WIN32_FIND_DATAW wfd;

	if (sDir.empty()) {
		return false;
	}
	
	sDir_w = utf8_to_utf16(sDir);
	if (INVALID_HANDLE_VALUE == (hFind = FindFirstFileW((sDir_w + L"\\*.*").c_str(), &wfd))) {
		return false;
	}
	
	do {
		sPath = sDir_w + L"\\" + wfd.cFileName;
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (L'.' != wfd.cFileName[0]) {
				if (!removeDir(utf16_to_utf8(sPath))) {
					return false;
				}
			}				
		} else {
			if (!DeleteFileW(sPath.c_str())) {
				return false;
			}
		}
	} while (FindNextFileW(hFind, &wfd));

	FindClose(hFind);

	if (!RemoveDirectoryW(sDir_w.c_str())) {
		return false;
	}

	return true;
}

} 