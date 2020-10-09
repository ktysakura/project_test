#include "file.h"
#include "FileInfo.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <io.h>
#include <fcntl.h>
#include <Windows.h>
namespace cbl{

CFile::CFile()
{
	init();
}
CFile::CFile(const char* szFilePath)
{
	init();
	m_sFilePath = szFilePath;
}
CFile::CFile(const string& sFilePath)
{
	init();
	m_sFilePath= sFilePath;
}	
CFile::~CFile()
{
	if (this->isOpen())
	{
		this->close();
	}
	
}
void CFile::init()
{
	m_sFilePath.clear();
	m_fp = NULL;
	m_bOpened = false;
}
int CFile::open(const char* szFileName, const char * szMode)
{
	if (szFileName == NULL || szMode == NULL)
	{
		return -1;
	}
#if _WIN32
	SetFileAttributesW(utf8_to_utf16(szFileName).c_str(), ((~FILE_ATTRIBUTE_HIDDEN) & (GetFileAttributesW(utf8_to_utf16(szFileName).c_str()))));
#endif
	errno_t err = _wfopen_s(&m_fp, utf8_to_utf16(szFileName).c_str(), utf8_to_utf16(szMode).c_str());
	if (err != 0 && m_fp == NULL)
	{
		printf("File::open(): errno:%d %s \n", err, strerror(errno));
		return -2;
	}

	::rewind(m_fp);
	m_bOpened = true;
	m_sFilePath = szFileName;

	return 0;
}

int CFile::open(const string& sFilePath, const char * szMode)
{
	return this->open(sFilePath.c_str(), szMode);
}

bool CFile::isExists() const
{
	// WIN32_FIND_DATA findData;
	// ::GetFileAttributesEx(fileName.c_str(), GetFileExInfoStandard,
	//	 reinterpret_cast<WIN32_FILE_ATTRIBUTE_DATA *>(&findData));
	return 0;
}

size_t CFile::getFileSize()
{
	int nFileSize, nCurPos;

	if ( ((nCurPos =  this->seek(0, SEEK_CUR)) < 0)
		|| ((nFileSize = this->seek(0, SEEK_END)) < 0)
		|| (this->seek(nCurPos, SEEK_SET) < 0)
		)
	{
		return -1;
	}

	return nFileSize;
}
size_t CFile::seek(size_t nOffset, size_t nWhence)
{
	if (m_fp == NULL || !this->isOpen())
	{
		return -1;
	}

	if (fseek(m_fp, nOffset, nWhence) != 0)
	{
		return -2;
	}

	return ftell(m_fp);
}

size_t CFile::truncate(size_t size)
{
	if (m_fp == NULL || size < 0)
	{
		return -1;
	}
	
	return _chsize(_fileno(m_fp), size) == 0 ? 0 : -2;
}

size_t CFile::truncate()
{
	if (m_fp == NULL)
	{
		return -1;
	}

	return truncate(ftell(m_fp));
}

long long CFile::getFileSize(const string& sFilePath)
{
	int ret;
	struct _stat64 statFile;
	if (sFilePath.empty()) {
		return -1;
	}
	
	if (ret = _wstati64(utf8_to_utf16(sFilePath).c_str(), &statFile) < 0) {
		return -2;
	}

	return statFile.st_size;
}

long long CFile::getFileCTime(const string& sFilePath)
{
	int ret = 0;
	struct stat statFile;
	if (sFilePath.empty()) {
		return -1;
	}

	ret = _access(sFilePath.c_str(), 0);
	if(ret < 0) {
		return 0;
	}

	ret = stat(sFilePath.c_str(), &statFile);
	if (ret < 0) {
		return -2;
	}

	return statFile.st_ctime * 1000;
}

size_t CFile::read(void* pBuffer, size_t nSize)
{
	int nReadSize = 0;

	if (pBuffer == NULL || nSize <= 0)
	{
		return -1;
	}

	if (m_fp == NULL || !this->isOpen())
	{
		return -2;
	}

	nReadSize = fread(pBuffer, 1, nSize, m_fp);
	if (nReadSize <= 0)
	{	
		return -3;
	}

	return nReadSize;
}

size_t CFile::read(string& sData, size_t size)
{
	sData.resize(size, '\x00');
	return this->read((void*)sData.data(), size);	
}
size_t CFile::write(const void* pBuffer, size_t nBufferSize)
{
	int nWriteSize = 0;

	if (pBuffer == NULL || nBufferSize <= 0)
	{
		return -1;
	}

	if (m_fp == NULL || !this->isOpen())
	{
		return -2;
	}

	nWriteSize = fwrite(pBuffer, 1, nBufferSize, m_fp);
	if (nWriteSize < 0)
	{
		return -3;
	}

	return nWriteSize;
}

size_t CFile::write(string& sData)
{
	return this->write(sData.data(), sData.size());	
}

size_t CFile::write(void *data, size_t size, int nWhence, size_t nOffset)
{
	string sData;
	size_t nCurPos, nFileSize;
	/* seek to offset */
	if((0 != nOffset) || (SEEK_SET != nWhence)){
		this->seek(nWhence, nOffset);
	}

	nCurPos = ftell(m_fp);
	nFileSize = this->getFileSize();
	if (nFileSize < 0)
	{
		return -1;
	}

	if (this->read(sData, nFileSize - nCurPos) < 0)
	{
		return -2;
	}

	sData.insert(0, (const char*)data, size);
	if (this->write(sData) < 0)
	{
		return -3;
	}

	return 0;
}

int CFile::close()
{
	if (m_fp == NULL)
	{
		return -1;
	}

	if (this->isOpen())
	{
		fclose(m_fp);
		m_bOpened = false;
		m_sFilePath.clear();
		m_fp = NULL;
	}

	return 0;
}
int CFile::flush()
{
	if (m_fp == NULL || !this->isOpen())
	{
		return -1;
	}
	return fflush(m_fp);
}
bool CFile::isOpen()
{
	return m_bOpened;
}

bool CFile::isExists(const string &fileName)
{
	return CFileInfo(fileName).isExists();
}

int CFile::rename(const string& oldName, const string& newName)
{
	
#ifdef _WIN32
	
	return ::MoveFileW(utf8_to_utf16(oldName).c_str(), utf8_to_utf16(newName).c_str()) ? 0 : -1;
#else //WIN32
	return ::rename(oldName.c_str(), newName.c_str());
#endif//LNIUX
}

int CFile::remove(const string& fileName)
{

#ifdef _WIN32
	return ::DeleteFileW(utf8_to_utf16(fileName).c_str()) ? 0 : -1;
#else //WIN32
	return ::unlink(fileName.c_str());
#endif //LINUX

}

}


