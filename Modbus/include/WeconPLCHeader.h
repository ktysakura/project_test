#pragma  once
#include "WeconPLCHeaderDefine.h"
#include "Md5Util.h"
#include <list>
#include <vector>

using namespace std;
long enlargeSize(long size);
class CWeconPLCHeader
{
public:
	CWeconPLCHeader();
	virtual ~CWeconPLCHeader();
	int package(PLC_FILE_NO fileNo, const list<FILE_ATTRIBUTE_T> &listAttribute, int nLimitSize, string &sData);
	int check(PLC_FILE_NO fileNo, const char* pBuffer, int nSize);
	int unpackage(const string &sData/*in*/, list<string> &listFileData);
public:
	/*´ò°übin*/
private:
	bool isCompressed(int nType);
	int compress(unsigned char* pDestBuf, unsigned long *pDestLen, const unsigned char* pSrcBuf, unsigned long pSrcLen);
private:
};
