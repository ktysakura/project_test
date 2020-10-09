#include "WeconPLCHeader.h"
#include "Md5Util.h"
#include "buffer.h"
#include "file.h"
#include "zlib.h"
#pragma comment(lib, "zlibstat")
#define DOWNLOAD_HEADER 1
using namespace cbl;
long enlargeSize(long size)
{
	long lRemainSize = 0;

	lRemainSize = size & (COMM_BUFFER_SIZE - 1);

	return lRemainSize > 0 ? (COMM_BUFFER_SIZE - lRemainSize) : 0;
}



CWeconPLCHeader::CWeconPLCHeader()
{
	
}

CWeconPLCHeader::~CWeconPLCHeader()
{

}

int CWeconPLCHeader::package(PLC_FILE_NO fileNo, const list<FILE_ATTRIBUTE_T> &listAttribute, int nLimitSize, string &sOutData)
{
#if DOWNLOAD_HEADER //打包下载头
	unsigned char headHash[MD5_LEN] = {0};
	int nSectionCount, nSize, ret, i, nOffset, nLoaderNumber, nRemainLen;
	unsigned char* pBuffer = NULL;
	uLongf comprLen;
	vector<SECTION_T> vecSection;
	string sHeaderData, sFileData;
	FILE_HEAD_T fileHeader = {0};
	string sHex;

	i = 0, ret = 0, nOffset = 0, nLoaderNumber = 0, nSize = 0;
	nSectionCount = listAttribute.size();
	memset(fileHeader.MagicNumber, 0, sizeof(fileHeader.MagicNumber));
	/*固定为WECON*/
	memcpy(fileHeader.MagicNumber, "WECON", 9);
	/*计算大文件头未补齐前的字节数*/
	fileHeader.HeadLen = sizeof(FILE_HEAD_T) + sizeof(SECTION_T) * nSectionCount + sizeof(headHash); //头长度
	fileHeader.HeadVer = 1;          //文件头版本
	fileHeader.FilewareVersion = 1;  //固件版本
	fileHeader.LadderLowVersion = 1; //最低要求的固件版本
	fileHeader.Save = 0;             //对齐保留
	fileHeader.Type = (unsigned short)fileNo; //文件号
	/*大文件头需要补齐的字节数*/
	nRemainLen = enlargeSize(fileHeader.HeadLen);
	/*分配4M缓冲区*/
	auto_deleter<void> buffer(memory_function::allocate(sizeof(char) * MALLOC_BUFFER_SIZE), memory_function::deallocate);
	if (buffer.data ==  NULL) {
		return -1;
	}

	pBuffer = (unsigned char *)buffer.data;
	/*section个数*/
	vecSection.resize(nSectionCount);
	for (list<FILE_ATTRIBUTE_T>::const_iterator iter = listAttribute.begin(); iter != listAttribute.end(); ++iter) {
		memset(pBuffer, 0, MALLOC_BUFFER_SIZE);
		comprLen = iter->sData.size();
		/*如果加载地址不为空(bin文件),加载数量加1*/
		if (iter->nExecAddr != LOADADDR_NONE) {
			++nLoaderNumber;
			/*如果bin文件大小大于最大限制，直接返回特殊错误标识*/
			if (iter->sData.size() > BIN_SIZE_LIMIT) {
				return ERROR_OUTOFRANGE_BIN;
			}
			sFileData.append(iter->sData);
		}
		else{
			if (PLC_TYPE_WCP == iter->type) {
				/*计算WCPhash值*/
				ret = md5Buffer((const unsigned char*)iter->sData.data(), iter->sData.size(), fileHeader.WcpHash);
				if (ret < 0) {
					ret = -3;
					goto _EXIT;
				}
			}
			/*是否可以压缩文件*/
			if (isCompressed(fileNo)) {
				/*compress file*/
				ret = this->compress(pBuffer, &comprLen, (const unsigned char*)iter->sData.c_str(), iter->sData.size());
				if (ret < 0) {
					ret = -4;
					goto _EXIT;
				}
				sFileData.append((const char*)pBuffer, comprLen);
			} else {
				sFileData.append(iter->sData);
			}
		}

		/*扩充每个文件大小*/
		nSize = enlargeSize(comprLen);
		sFileData.append(nSize, '\x00');
		/*偏移地址*/
		vecSection[i].Offset = fileHeader.HeadLen + nOffset + nRemainLen;
		vecSection[i].ExecAddr = iter->nExecAddr;
		/*文件真实长度*/
		vecSection[i].SecFileSize = comprLen;
		/*拼接大文件头*/
		sHeaderData.append((const char *)&vecSection[i], sizeof(SECTION_T));
		nOffset = sFileData.size();
		++i;
	}

	fileHeader.AllFileSize = sFileData.size();
	fileHeader.LoaderNumber = nLoaderNumber;

	/*填充文件hash*/
	ret = md5Buffer((const unsigned char*)sFileData.data(), sFileData.size(), fileHeader.FileHash);
	if (ret < 0) {
		ret = -5;
		goto _EXIT;
	}
	
	sHeaderData.insert(0, (const char *)&fileHeader, sizeof(FILE_HEAD_T));	
	/*计算大文件头hash*/
	ret = md5Buffer((const unsigned char*)sHeaderData.data(), fileHeader.HeadLen - MD5_LEN, headHash);
	hexBuffer(headHash, MD5_LEN, false, sHex);
	printf("download head hash:%s\n", sHex.c_str());
	if (ret < 0) {
		ret = -6;
		goto _EXIT;
	}
	/*拼接大文件头的hash*/
	sHeaderData.append((const char *)headHash, sizeof(headHash));
	/*补齐大文件头剩余长度*/
	sHeaderData.append(nRemainLen, '\x00');
	sOutData.append(sHeaderData);
	sOutData.append(sFileData);
	printf("download total size:(%d)KB\n", (sOutData.size() + KB - 1) / KB);
	if (nLimitSize != UNCHECK_LIMIT) {
		/*检查文件大小限制*/
		if ((int)(sOutData.size()) > nLimitSize) {
			return ERROR_OUTOFRANGE;
		}
	}

_EXIT:
	return ret;
#else //不打包头,直接下载
	int nSize;
	sOutData = listAttribute.front().sData;
	nSize = enlargeSize(sOutData.size());
	sOutData.append(nSize, '\x00');
	return 0;
#endif
}

int CWeconPLCHeader::unpackage(const string &sData/*in*/, list<string> &listFileData)
{
	int nSectionCount, i;
	FILE_HEAD_T* pFileHead =  NULL;
	const SECTION_T* pSection = NULL;
	const unsigned char * pHead = NULL;
	unsigned char szFileHash[MD5_LEN] = { 0 };
	string sTemp;
	Bytef *pBuffer;
	uLongf uncomprLen;
	
	nSectionCount = 0;
	pHead = (const unsigned char *)sData.c_str();
	pFileHead = (FILE_HEAD_T*)pHead;
	/*判断文件长度是否相等*/
	if ((sData.size() - ((SECTION_T*)(pHead + sizeof(FILE_HEAD_T)))->Offset) != pFileHead->AllFileSize){
		return -1;
	}

	/*计算文件hash*/
	if (md5Buffer(pHead + ((SECTION_T*)(pHead + sizeof(FILE_HEAD_T)))->Offset, pFileHead->AllFileSize, szFileHash) < 0){
		return -2;
	}

	/*检查hash值*/
	if (memcmp(pFileHead->FileHash, szFileHash, MD5_LEN)) {
		return -3;
	}
	
	/*获取section个数*/
	nSectionCount = (pFileHead->HeadLen - sizeof(FILE_HEAD_T) - MD5_LEN) / sizeof(SECTION_T);	
	pSection = (const SECTION_T*)(pHead + sizeof(FILE_HEAD_T));
	for (i = 0; i < nSectionCount; i++) {
		/*是否被压缩*/
		if (pSection[i].ExecAddr == LOADADDR_NONE && isCompressed(pFileHead->Type)) {
			/*check uncompress size > MALLOC_BUFFER_SIZE*/
			if (pSection[i].SecFileSize > MALLOC_BUFFER_SIZE) {
				return -4;
			}

			/*allocate memory*/
			auto_deleter<void> buffer(memory_function::allocate(sizeof(Bytef) * MALLOC_BUFFER_SIZE), memory_function::deallocate);
			if (buffer.data == 0) {
				return -5;
			}

			pBuffer = (Bytef *)buffer.data;
			if (uncompress(pBuffer, &uncomprLen, pHead + pSection[i].Offset, pSection[i].SecFileSize) < 0) {
				return -6;
			}
			sTemp.assign((const char*)pBuffer, uncomprLen);
		} else {
			sTemp.assign((const char*)pHead + pSection[i].Offset, pSection[i].SecFileSize);
		}
		listFileData.push_back(sTemp);
	}

	return 0;
}
//上传功能时要先检查大文件的hash
int CWeconPLCHeader::check(PLC_FILE_NO fileNo, const char* pBuffer, int nSize)
{
	string sHeadHashBefore, sHeadHashAfter;
	unsigned char szHeadHash[MD5_LEN] = { 0 };
	const FILE_HEAD_T *pFileHead = (const FILE_HEAD_T *)pBuffer;
	
	if ((nSize < sizeof(FILE_HEAD_T) + sizeof(SECTION_T) + MD5_LEN)){
		return -1;
	}

	if (pFileHead->Type != fileNo) {
		return -2;
	}

	/*计算大文件头的hash*/
	if (md5Buffer((const unsigned char*)pBuffer, pFileHead->HeadLen - MD5_LEN, szHeadHash) < 0) {
		return -2;
	}

	/*比较大文件头的hash*/
	if (memcmp(pBuffer + pFileHead->HeadLen - MD5_LEN, szHeadHash, MD5_LEN) != 0) {
		return -3;
	}

#if _DEBUG
	hexBuffer(pBuffer + pFileHead->HeadLen - MD5_LEN, MD5_LEN, false, sHeadHashBefore);
	hexBuffer(szHeadHash, MD5_LEN, false, sHeadHashAfter);
	printf("head md5 before download: %s\n", sHeadHashBefore.c_str());
	printf("head md5 after upload: %s\n", sHeadHashAfter.c_str());
#endif

	return 0;

}

bool CWeconPLCHeader::isCompressed(int nType)
{
	 return (nType == PLC_BINWCP_NO || nType == PLC_COMMENT_NO);
}

int CWeconPLCHeader::compress(unsigned char* pDestBuf, unsigned long *pDestLen, const unsigned char* pSrcBuf, unsigned long pSrcLen)
{
	return compress2(pDestBuf, pDestLen, (const Bytef*)pSrcBuf, (uLongf)pSrcLen, Z_BEST_COMPRESSION);
}

