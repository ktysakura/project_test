#include "FileInfo.h"
#include "str.h"
#include "encode.h"
#include <Windows.h>
namespace cbl {


static inline bool getFindData(string path, WIN32_FIND_DATA &findData)
{
	if (isEndWith(path, "\\\t\r")) {
		path = rtrim(path, "\\\t\r");
	}

	// can't handle drives
	if (isEndWith(path, ":")) {
		return false;
	}
	
	HANDLE hFind = ::FindFirstFile(utf8_to_utf16(path).c_str(), &findData);
	if (hFind == INVALID_HANDLE_VALUE) {
		return false;
	}
	::FindClose(hFind);

	return true;
}

CFileInfo::CFileInfo()
{

}

CFileInfo::CFileInfo(const string &filePath)
{
	m_filePath = filePath;
}

CFileInfo::~CFileInfo()
{

}

bool CFileInfo::isExists() const
{
	WIN32_FIND_DATA data;
	return getFindData(m_filePath, data) && ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0); 
}

}