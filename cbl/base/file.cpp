#include "file.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include "str.h"
#include "dir.h"
#include "DataType.h"
#include "encode.h"

//最大读一行的长度
#define MAX_LINE_LENGTH		(8 * 1024)

namespace cbl {
	
int findBOM(const unsigned char *szBuffer, int nSize, char *szEncodeFormat)
{
	if ((NULL == szBuffer) || ((nSize) < 2)) {
		return -1;	
	}
		
	if (memcmp(szBuffer, "\xEF\xBB\xBF", 3) == 0) {
		if (szEncodeFormat) {
			strcpy(szEncodeFormat, "UTF8");
		}
		return 3;
	} else if (memcmp(szBuffer, "\xFF\xFE", 2) == 0) {
		if (szEncodeFormat) {
			strcpy(szEncodeFormat, "UTF16-LE");
		}
		return 2;
	} else if (memcmp(szBuffer, "\xFE\xFF", 2) == 0) {
		if (szEncodeFormat) {
			strcpy(szEncodeFormat, "UTF16-BE");
		}
	}

	return -2;
}

int readTextFile(const string &sFileName, list<string> *plistLine)
{
	int nRtn;
	FILE *fp = NULL;
	wstring sFileName_w;
	
	sFileName_w = utf8_to_utf16(sFileName);

	fp = _wfopen(sFileName_w.c_str(), L"r");
	if (NULL == fp) {
		return -1;	
	}
	
	if ((nRtn = readTextFile(fp, plistLine)) < 0) {
		fclose(fp);
		return -2;	
	}
	
	fclose(fp);
	
	return nRtn;
}

int readTextFile(FILE *fp, list<string> *plistLine)
{
	int nLineCount = 0;
	
	if (NULL == fp) {
		return -1;
	}
	
	if (plistLine) {
		plistLine->clear();	
	}
	
	while (1) {
		//如果文本行超过MAX_LINE_LENGTH，该函数会有问题，会把一行分成两行
		char szLine[MAX_LINE_LENGTH] = {0};
		
		if (NULL == fgets(szLine, sizeof(szLine), fp)) {
			break;	
		}
		
		if (0 == nLineCount) {
			int nBomBytes = findBOM((unsigned char *)szLine, strlen(szLine), NULL);

			if (nBomBytes > 0) {
				strcpy(szLine, &szLine[nBomBytes]);	
			}
		}
		
		if ((int)strlen(removeCRLF(szLine)) > 0) {
			if (plistLine) {
				plistLine->push_back(szLine);	
			}				
			nLineCount++;			
		}
	}
	
	return nLineCount;	
}

int readTextFile(const string &sFileName, PFONNEWLINE pfOnNewLine, void *parameter)
{
	int nRtn;
	FILE *fp = NULL;
	wstring sFileName_w;

	if (sFileName.empty()) {
		return -1;
	}

	sFileName_w = utf8_to_utf16(sFileName);

	fp = _wfopen(sFileName_w.c_str(), L"r");
	if (NULL == fp) {
		return -2;	
	}
	
	if ((nRtn = readTextFile(fp, pfOnNewLine, parameter)) < 0) {
		fclose(fp);
		return -3;	
	}
	
	fclose(fp);
	
	return nRtn;	
}

int readTextFile(FILE *fp, PFONNEWLINE pfOnNewLine, void *parameter)
{
	int nLineCount = 0;
	
	if (NULL == fp) {
		return -1;
	}
	
	while (1) {
		//如果文本行超过MAX_LINE_LENGTH，该函数会有问题，会把一行分成两行
		char szLine[MAX_LINE_LENGTH] = {0};
		
		if (NULL == fgets(szLine, sizeof(szLine), fp)) {
			break;	
		}
		
		if (0 == nLineCount) {
			int nBomBytes = findBOM((unsigned char *)szLine, strlen(szLine), NULL);
			
			if (nBomBytes > 0) {
				strcpy(szLine, &szLine[nBomBytes]);	
			} 
		}
		
		if ((int)strlen(removeCRLF(szLine)) > 0) {
			if (pfOnNewLine) {
				pfOnNewLine(szLine, parameter);		
			}
			nLineCount++;			
		}
	}
	
	return nLineCount;	
}

int locateFile(const string &sFilePath, bool bEnableRead, bool bEnableWrite, int nOffset, int nWhence, int &fd)
{
	int nOpenFlags, fdTemp = -1;
	
	if (sFilePath.empty() || ((!bEnableRead) && (!bEnableWrite))) {
		return -1;
	}
	
	if (bEnableRead && bEnableWrite) {
		nOpenFlags = O_RDWR | O_CREAT;
	} else if (bEnableRead) {
		nOpenFlags = O_RDONLY;
	} else {
		nOpenFlags = O_WRONLY | O_CREAT;
	}
	
	if ((fdTemp = _wopen(utf8_to_utf16(sFilePath).c_str(), nOpenFlags, 0600)) < 0) {
		return -2;
	}
	
	if ((0 != nOffset) || (SEEK_SET != nWhence)) {
		if (lseek(fdTemp, nOffset, nWhence) < 0) {
			close(fdTemp);
			return -3;	
		}
	}
	
	fd = fdTemp;
	return 0;
}

int writeToFile(const void *pBuffer, size_t nSize, int fd)
{
	char *pHead = (char *)pBuffer;
	int nRemainSize, nNeedWriteBytes, nMaxWriteBytes = (1 << 16);
	
	if ((NULL == pBuffer) || (fd < 0)) {
		return -1;	
	}
			
	nRemainSize = nSize;

	while (nRemainSize > 0) {
		nNeedWriteBytes = cbl_min(nRemainSize, nMaxWriteBytes);
		if (nNeedWriteBytes != (int)write(fd, pHead, nNeedWriteBytes)) {
			return -2;
		}
		nRemainSize -= nNeedWriteBytes;
		pHead += nNeedWriteBytes;
	}	
	
	return nSize;
}

int writeToFile(const string &sData, int fd)
{
	return writeToFile(sData.c_str(), sData.size(), fd);
}

long long getFileSize(const string &sFilePath)
{
	int fd;
	long long llFileSize;
	
	if (locateFile(sFilePath, true, false, 0, SEEK_SET, fd) < 0) {
		return -1;	
	}
	
	if ((llFileSize = _lseeki64(fd, 0, SEEK_END)) < 0) {
		close(fd);
		return -2;	
	}
	
	close(fd);
	return llFileSize;
}

long long getFileSize(int fd)
{
	long long llCurrentPos, llFileSize;
	
	if(fd < 0) {
		return -1;	
	}
	
	if (((llCurrentPos = _lseeki64(fd, 0, SEEK_CUR)) < 0) || 
		((llFileSize = _lseeki64(fd, 0, SEEK_END)) < 0) || 
		(_lseeki64(fd, llCurrentPos, SEEK_SET) < 0)) {
		return -2;
	}
		
	return llFileSize;
}

int loadFromFile(const string &sFilePath, int nOffset, int nWhence, int nSize, unsigned char **ppBuffer, int nLimitSize)
{
	int fd = -1;
	long long llFileSize;
    int nLoadSize, nRemainSize, nReadBytes, nExitCode = 0;
    unsigned char *pBuffer = NULL;
    unsigned char *pHead = NULL;

    if (sFilePath.empty() || NULL == ppBuffer) {
        return -1;
    }

    if (locateFile(sFilePath, true, false, nOffset, nWhence, fd) < 0) {
        return -2;
    }

    if ((llFileSize = getFileSize(fd)) < 0) {
    	nExitCode = -3;
        goto __EXIT_LOAD_FROM_FILE;
    }
    
    if (nSize > 0) {
    	nLoadSize = cbl_min(int(llFileSize - nOffset), nSize);
    } else {
    	nLoadSize = int(llFileSize - nOffset);
    }
    
    if (((nLimitSize > 0) && (nLoadSize > nLimitSize))) {
        nExitCode = -4;
        goto __EXIT_LOAD_FROM_FILE;
    }
	
	if (nLoadSize > 0) {
	    if (NULL == (pBuffer = (unsigned char *)malloc(nLoadSize))) {
	        nExitCode = -5;
	        goto __EXIT_LOAD_FROM_FILE;
	    }
 
	    pHead = pBuffer;
	    nRemainSize = nLoadSize;
	    while (nRemainSize > 0) {
			if ((nReadBytes = (int)read(fd, pHead, nRemainSize)) <= 0) {
		        nExitCode = -6;
		        goto __EXIT_LOAD_FROM_FILE;
		    }
		    nRemainSize -= nReadBytes;
		    pHead += nReadBytes; 	
	    }

	    *ppBuffer = pBuffer;		
	}
	
    nExitCode = nLoadSize;

__EXIT_LOAD_FROM_FILE:
    if (nExitCode <= 0) {
        if (pBuffer) {
            free(pBuffer);
        }
    }

    if (fd >= 0) {
        close(fd);
    }

    return nExitCode;	
}

int loadFromFile(const string &sFilePath, int nOffset, int nWhence, int nSize, string &sData, int nLimitSize)
{
	int nLoadSize;
	unsigned char *pBuffer = NULL;
	
	if ((nLoadSize = loadFromFile(sFilePath, nOffset, nWhence, nSize, &pBuffer, nLimitSize)) < 0) {
		return -1;
	}

	if (nLoadSize > 0) {
		sData.assign((char *)pBuffer, nLoadSize);	
	} else {
		sData.clear();
	}
	
	if (pBuffer) {
		free(pBuffer);		
	}
	
	return nLoadSize;
}

int loadFromFile(const string &sFilePath, unsigned char **ppBuffer, int nLimitSize)
{
	return loadFromFile(sFilePath, 0, SEEK_SET, -1, ppBuffer, nLimitSize);
}

int loadFromFile(const string &sFilePath, string &sData, int nLimitSize)
{
	return loadFromFile(sFilePath, 0, SEEK_SET, -1, sData, nLimitSize);
}

int saveToFile(const void *pBuffer, int nSize, const string &sFilePath)
{
	int nRtn, fd;
	string sDir = dirName(sFilePath);
	
	if ((NULL == pBuffer) || (nSize < 0)) {
		return -1;
	}
	
	if ((!isDir(sDir)) && (!makeDir(sDir))) {
		return -2;
	}
	
    if ((nRtn = locateFile(sFilePath, false, true, 0, SEEK_SET, fd)) < 0) {
        return -3;
    }
	
	if (nSize != writeToFile(pBuffer, nSize, fd)) {
		close(fd);
		return -4;	
	}
	
	if (_chsize_s(fd, nSize) < 0) {
		close(fd);
		return -5;
	}
	
	close(fd);
	return 0;
}

int saveToFile(const string &sData, const string &sFilePath)
{
	return saveToFile(sData.c_str(), (int)sData.size(), sFilePath);
}

}
