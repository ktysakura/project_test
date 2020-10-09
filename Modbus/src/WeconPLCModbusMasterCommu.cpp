
#include "WeconPLCModbusMasterCommu.h"
#include "Md5Util.h"
#include "file.h"
#include "zlib.h"
#include "WeconPLCHeaderDefine.h"
#include <time.h>
#define COMM_FILE_BEGIN(key) \
do{\
	if (!sPassword.empty()){\
		ret = this->decTemporary(key, sPassword);\
		if (ret < 0){\
			printf("decTemporary error:%d\n", ret);\
			goto __ERROR_EXIT;\
		}\
	}\
} while (0)

#define COMM_FILE_END(key) \
do{\
	if (!sPassword.empty()){\
		this->removeDecTemporary(key);\
	}\
} while (0)

using namespace cbl;
int CWeconPLCModbusMasterCommu::writeBitInRegister(int nAddress, int bit, bool bOn, int nTimeout)
{
	unsigned char szSendBuffer[1024] = { 0 };
	unsigned char szRecvBuffer[1024] = { 0 };
	int nRtn, nDataSize, nRecvBytes;
	int nHopeSize = 0;
	/* package write subcode */
	nDataSize = m_modbusMaster.packWriteBitInRegister(nAddress, bit, bOn, szSendBuffer, sizeof(szSendBuffer));
	if (nDataSize < 0) {
		printf("modbus packageMutliSubCode() error %d\n", nDataSize);
		return -1;
	}

	/*get hope recv size*/
	nHopeSize = m_modbusMaster.getSubRecvSize(WECON_PLC_FUNCTION_CODE_WRITE, FUNCTION_SUB_CODE_WRITE_BITINREGISTER);
	/* do commu */
	nRecvBytes = this->doCommu(szSendBuffer, nDataSize, szRecvBuffer, sizeof(szRecvBuffer), nHopeSize, nTimeout);
	if (nRecvBytes <= 0) {
		printf("modbus writeSubCode doCommu() error %d\n", nRecvBytes);
		return -2;
	}

	/* unpackage write subcode */
	if ((nRtn = m_modbusMaster.unpackWriteBitInRegister(szRecvBuffer, nRecvBytes, nAddress, bit, bOn)) < 0) {
		printf("modbus unpackageMutliSubCode() error %d\n", nRtn);
		return -3;
	}

	return 0;
}
/* write sub code
**/
int CWeconPLCModbusMasterCommu::writeSubCode(int nSubCode/*in*/, int nTimeout/*in*/)
{
	unsigned char szSendBuffer[1024] = { 0 };
	unsigned char szRecvBuffer[1024] = { 0 };
	int nRtn, nDataSize, nRecvBytes;
	int nHopeSize = 0, nRecvSubCode;

	/* package write subcode */
	if ((nDataSize = m_modbusMaster.packageMutliSubCode(nSubCode, szSendBuffer, sizeof(szSendBuffer))) < 0){
		printf("modbus packageMutliSubCode() error %d\n", nDataSize);
		return -1;
	}

	/*get hope recv size*/
	nHopeSize = m_modbusMaster.getSubRecvSize(WECON_PLC_FUNCTION_CODE_WRITE_SUB_CODE, nSubCode);
	/* do commu */
	if ((nRecvBytes = this->doCommu(szSendBuffer, nDataSize, szRecvBuffer, sizeof(szRecvBuffer), nHopeSize, nTimeout)) <= 0){
		printf("modbus writeSubCode doCommu() error %d\n", nRecvBytes);
		return -2;
	}

	/* unpackage write subcode */
	if ((nRtn = m_modbusMaster.unpackageMutliSubCode(szRecvBuffer, nRecvBytes, nRecvSubCode)) < 0){
		printf("modbus unpackageMutliSubCode() error %d\n", nRtn);
		return -3;
	}

	if (nSubCode != nRecvSubCode){
		printf("modbus writeSubCode() diff subcode(%d,%d)\n", nSubCode, nRecvSubCode);
		return -4;
	}

	return 0;
}

/* write clock
**/
int CWeconPLCModbusMasterCommu::writeClock(const struct tm &tm_Time, int nTimeout /*= 1000*/)
{
	vector<int> vecValue(7);

	vecValue.assign((int*)&tm_Time,  ((int*)&tm_Time) + vecValue.size());
	
	return this->writeInterior(0x1000, vecValue, nTimeout);
}

/* read clock
**/
int CWeconPLCModbusMasterCommu::readClock(struct tm &tmTime, int nTimeout /*= 1000*/)
{
	int ret;
	int arr[7] = { 0 };

	vector<int> vecValue;

	ret = this->readRegisters(0x5000 + 100, 7, vecValue, nTimeout);
	if (ret < 0) {
		return ret;
	}

	copy(vecValue.begin(), vecValue.end(), arr);
	memcpy(&tmTime, arr, sizeof(arr));
	if (tmTime.tm_year < 2000) {
		tmTime.tm_year += 2000;
	}

	return 0;
}

/*write password
**/
int CWeconPLCModbusMasterCommu::writePassword(PASSWORD_KEY_TYPE keyType, const string& sPassword, int nTimeout)
{
	int ret = 0;
	string sKeySeed;
	int nCount = 3;
	do
	{
		ret = this->operatePassword(FUNCTION_SUB_CODE_WRITE_PASSWORD, keyType, sPassword, nTimeout);
		if (ret == 0){
			break;
		}
	}while(--nCount);
	
	return ret;
}

/* decrypt password temporarily
**/
int  CWeconPLCModbusMasterCommu:: decTemporary(PASSWORD_KEY_TYPE keyType, const string& sPassword, int nTimeout /*= 1000*/)
{
	return this->operatePassword(FUNCTION_SUB_CODE_TEMPORARY_DECRYPT, keyType, sPassword, nTimeout);
}
int CWeconPLCModbusMasterCommu::checkPassword(PASSWORD_KEY_TYPE keyType, const string& sPassword, int &nCode, int nTimeout)
{
	return this->operatePassword(FUNCTION_SUB_CODE_CHECK_PASSWORD, keyType, sPassword, nCode, nTimeout);
}
int CWeconPLCModbusMasterCommu::verifyPassword(PASSWORD_KEY_TYPE keyType, const string& sPassword, int &nCode, int nTimeout /*= 1000*/)
{
	return this->operatePassword(FUNCTION_SUB_CODE_VERIFY_PASSWORD, keyType, sPassword, nCode, nTimeout);
}

int CWeconPLCModbusMasterCommu::removePassword(PASSWORD_KEY_TYPE keyType, const string& sPassword, int nTimeout /*= 1000*/)
{
	return this->operatePassword(FUNCTION_SUB_CODE_REMOVE_PASSWORD, keyType, sPassword, nTimeout);
}
	
/* read encrypt info
**/
int CWeconPLCModbusMasterCommu::readEncryptInfo(int nIndex, int nCount, map<PASSWORD_KEY_TYPE, bool> &mapEncryptInfo, int nTimeout /*= 1000*/)
{
	int i, nSize;
	string sData;
	const unsigned char *pHead = NULL;
	PASSWORD_KEY_TYPE keyType;
	bool bEncrypt;

	mapEncryptInfo.clear();
	if (this->readPLCInfo(FUNCTION_SUB_CODE_READ_ENCRYPT_MESSAGE, nIndex, nCount, sData, nTimeout) < 0)
	{
		return -1;
	}

	nSize = sData.size();
	pHead = (const unsigned char *)sData.data();

	if (*pHead != nIndex)
	{
		return -2;
	}

	for (i = 0; i <  nSize;)
	{
		keyType = (PASSWORD_KEY_TYPE)(*(pHead + i));
		bEncrypt = (*(pHead + i + 1) == 1);
		mapEncryptInfo.insert(pair<PASSWORD_KEY_TYPE, bool>(keyType, bEncrypt));	
		i+=2;
	}

	return mapEncryptInfo.size();
}


int CWeconPLCModbusMasterCommu::getKeySeed(string& sKeySeed)
{
	string sRandomSeed;
	unsigned char szCommKey[ENCRY_KEY_LEN] = { 0 };

	if (this->readRandomSeed(sRandomSeed) < 0)
	{
		return -1;
	}
	m_xor.GetCommuiKey(szCommKey, (const unsigned char*)sRandomSeed.c_str());
	sKeySeed.assign((const char *)szCommKey, ENCRY_KEY_LEN);

	return 0;
}
/* read encrypt info
**/
int CWeconPLCModbusMasterCommu::readRandomSeed(string &sRandomSeed, int nTimeout /*= 1000*/)
{
	return this->readPLCInfo(FUNCTION_SUB_CODE_READ_RANDOM_SEED, 0, 8, sRandomSeed, nTimeout);
}

int CWeconPLCModbusMasterCommu::removeDecTemporary(PASSWORD_KEY_TYPE keyType, int nTimeout /*= 1000*/)
{
	vector<char> vecKey(1, (int)keyType);
	return this->removeDecTemporary(vecKey, nTimeout);
}
/* remove decrypt temporary
**/
int CWeconPLCModbusMasterCommu::removeDecTemporary(const vector<char>& vecKey, int nTimeout/*= 1000*/)
{
	unsigned char szSendBuffer[1024] = { 0 };
	unsigned char szRecvBuffer[1024] = { 0 };
	int nRtn, nDataSize, nRecvBytes, nHopeSize;
	
	/* package package remove decrypt temporary*/
	nDataSize = m_modbusMaster.packageRemoveDecTemporary(vecKey, szSendBuffer, sizeof(szSendBuffer));
	if (nDataSize < 0)
	{
		return -1;
	}

	/*get hope size*/
	nHopeSize = m_modbusMaster.getSubRecvSize(WECON_PLC_FUNCTION_CODE_WRITE, FUNCTION_SUB_CODE_REMOVE_TEMPORARY_DECRYPT);

	/*do comm*/
	nRecvBytes = this->doCommu(szSendBuffer, nDataSize, szRecvBuffer, sizeof(szRecvBuffer), nHopeSize, nTimeout);
	if (nRecvBytes < 0)
	{
		return -2;
	}

	/*unpackage remove dec temporary*/
	nRtn = m_modbusMaster.unpackageRemoveDecTemporary(szRecvBuffer, nRecvBytes, vecKey);

	if (nRtn < 0)
	{
		return -3;
	}

	return 0;
}

/* read plc info
**/
int CWeconPLCModbusMasterCommu::readPLCInfo(int nSubCode, int nIndex, int nCount, string& sData, int nTimeout)
{
	unsigned char szSendBuffer[1024] = { 0 };
	unsigned char szRecvBuffer[1024] = { 0 };
	int nRtn, nDataSize, nRecvBytes, nHopeSize;
	
	/* package write subcode */
	nDataSize = m_modbusMaster.packageReadPLCInfo(nSubCode, nIndex, nCount, szSendBuffer, sizeof(szSendBuffer));
	if (nDataSize < 0)
	{
		return -1;
	}

	/* get hope size */
	nHopeSize = m_modbusMaster.getSubRecvSize(WECON_PLC_FUNCTION_CODE_READ, nSubCode, nCount);

	/*do comm*/
	nRecvBytes = this->doCommu(szSendBuffer, nDataSize, szRecvBuffer, sizeof(szRecvBuffer), nHopeSize, nTimeout);
	if (nRecvBytes < 0)
	{
		return -2;
	}
	
	/*unpackage read plc info*/
	nRtn = m_modbusMaster.unpackageReadPLCInfo(szRecvBuffer, nRecvBytes, nSubCode, nCount, sData);
	if (nRtn < 0)
	{
		return -3;
	}

	return 0;
}

int CWeconPLCModbusMasterCommu::operatePassword(int nSubCode, unsigned char cKeyType, const string& sPassword, int nTimeout /*= 1000*/)
{
	int nCode;
	return this->operatePassword(nSubCode, cKeyType, sPassword, nCode, nTimeout);
}
/* operate password
**/
int CWeconPLCModbusMasterCommu::operatePassword(int nSubCode, unsigned char cKeyType, const string& sPassword, int &nCode, int nTimeout)
{
	unsigned char szSendBuffer[1024] = { 0 };
	unsigned char szRecvBuffer[1024] = { 0 };
	unsigned char szEncBuffer[512] = { 0 };
	string sKeySeed;
	int nRtn, nDataSize, nRecvBytes, nHopeSize;
	
	
	nRtn = this->getKeySeed(sKeySeed);
	if (nRtn < 0){
		return -1;
	}
	/*package operate password*/
	nDataSize = m_modbusMaster.packageOperatePassword(nSubCode, cKeyType, sPassword, sKeySeed, szSendBuffer, sizeof(szSendBuffer));
	if(nDataSize < 0){
		return -2;
	}

	/*get hope size*/
	nHopeSize = m_modbusMaster.getSubRecvSize(WECON_PLC_FUNCTION_CODE_WRITE, nSubCode);

	/*do comm*/
	nRecvBytes = this->doCommu(szSendBuffer, nDataSize, szRecvBuffer, sizeof(szRecvBuffer), nHopeSize, nTimeout); 
	if (nRecvBytes < 0)	{
		return -3;
	}

	/*unpackage operate password*/
	nRtn = m_modbusMaster.unpackageOperatePassword(szRecvBuffer, nRecvBytes, nSubCode, nCode);
	if (nRtn < 0){
		return -4;
	}
	
	return 0;
}

/* write interinor
**/
int CWeconPLCModbusMasterCommu::writeInterior(int nAddress, const vector<int>& vecValue, int nTimeout /*= 1000*/)
{
	unsigned char szSendBuffer[1024] = { 0 };
	unsigned char szRecvBuffer[1024] = { 0 };
	int nRtn, nDataSize, nRecvBytes;
	int nHopeSize = 0;

	/* package write interior */
	if ((nDataSize = m_modbusMaster.packageWriteInterior(nAddress, vecValue,  szSendBuffer, sizeof(szSendBuffer))) < 0)
	{
		printf("modbus packageMutliSubCode() error %d\n", nDataSize);
		return -1;
	}

	/*get hope recv size*/
	nHopeSize = m_modbusMaster.getSubRecvSize(WECON_PLC_FUNCTION_CODE_WRITE, FUNCTION_SUB_CODE_RW_INTERIOR);

	/* do commu */
	if ((nRecvBytes = this->doCommu(szSendBuffer, nDataSize, szRecvBuffer, sizeof(szRecvBuffer), nHopeSize, nTimeout)) <= 0)
	{
		printf("modbus writeSubCode doCommu() error %d\n", nRecvBytes);
		return -2;
	}

	/*unpackage write interior*/
	if ((nRtn = m_modbusMaster.unpackageWriteInterior(szRecvBuffer, nRecvBytes, nAddress, vecValue.size())) < 0)
	{
		printf("modbus unpackageWriteInterior() error %d\n", nRtn);
		return -3;
	}

	return 0;
}

/* read interior
**/
int CWeconPLCModbusMasterCommu::readInterior(int nAddress, int nCount,  vector<int>& vecValue, int nTimeout /*= 1000*/)
{
	unsigned char szSendBuffer[1024] = { 0 };
	unsigned char szRecvBuffer[1024] = { 0 };
	int nRtn, nDataSize, nRecvBytes;
	int nHopeSize = 0;

	/* package write subcode */
	if ((nDataSize = m_modbusMaster.packageReadInterior(nAddress, nCount,  szSendBuffer, sizeof(szSendBuffer))) < 0)
	{
		printf("modbus packageMutliSubCode() error %d\n", nDataSize);
		return -1;
	}

	/*get hope recv size*/
	nHopeSize = m_modbusMaster.getSubRecvSize(WECON_PLC_FUNCTION_CODE_READ, FUNCTION_SUB_CODE_RW_INTERIOR, nCount);

	/* do commu */
	if ((nRecvBytes = this->doCommu(szSendBuffer, nDataSize, szRecvBuffer, sizeof(szRecvBuffer), nHopeSize, nTimeout)) <= 0)
	{
		printf("modbus writeSubCode doCommu() error %d\n", nRecvBytes);
		return -2;
	}

	/*unpackage write interior*/
	if ((nRtn = m_modbusMaster.unpackageReadInterior(szRecvBuffer, nRecvBytes, nAddress, nCount, vecValue)) < 0)
	{
		printf("modbus unpackageMutliSubCode() error %d\n", nRtn);
		return -3;
	}

	return 0;
}

/* write file
**/
int CWeconPLCModbusMasterCommu::writeFile(int fileNo, const unsigned char *pBuffer, unsigned long lFileSize, int nTimeout, bool bExWrite /*= false*/)
{
	unsigned long lRemainSize;
	unsigned char szBuffer[COMM_BUFFER_SIZE] = {0};
	int nRecordNo = 0, nTotalCount, ret;
	const FILE_HEAD_T *pHead = NULL;
	string sKeySeed;
	
	if ( ((pHead = (const FILE_HEAD_T *)pBuffer) == NULL ) 
		|| (lFileSize <= 0)
		|| (fileNo < PLC_BINWCP_NO)
		|| (fileNo >= FILE_NUMMAX)
		|| (lFileSize & (COMM_BUFFER_SIZE - 1))) { /* lFileSize 必须是0x80倍数 */
		return -1;
	}

	lRemainSize = lFileSize;
	/*获取密钥种子*/
	ret = this->getKeySeed(sKeySeed);
	if (ret < 0) {
		return -2;
	}

	/*计算总共要发送的次数*/
	nTotalCount = (lFileSize + COMM_BUFFER_SIZE - 1) / COMM_BUFFER_SIZE;
	while (lRemainSize > 0) {
		if (m_commFunc.on_commu_run) {
			/*更新进度条*/
			ret = m_commFunc.on_commu_run(nRecordNo, nTotalCount, m_commFunc.arg);
			if (ret < 0) {
				/*发送中断消息*/
				if (this->writeSubCode(FUNCTION_SUB_CODE_DOWNLOAD_INT, 1000) < 0){
					return ret;
				}
				return ret;
			}
		}

		/*加密数据，0x80的倍数*/
		ret = m_xor.EncData(pBuffer, szBuffer, sKeySeed, COMM_BUFFER_SIZE);
		if (ret < 0) {
			return -4;
		}

		/*发送文件数据*/
		ret = this->writeFileData(fileNo, nRecordNo, szBuffer, COMM_BUFFER_SIZE, nTimeout, bExWrite);
		if(ret < 0) {
			return -5;
		}
		pBuffer += COMM_BUFFER_SIZE;
		lRemainSize -= COMM_BUFFER_SIZE;
		++nRecordNo;	
	}

	return lRemainSize;
}

/* write file record
**/
int CWeconPLCModbusMasterCommu::writeFileData(int nFileNo/*in*/, int nRecordNo/*in*/, unsigned char *pBuffer/*in*/, int nBufferSize/*in*/, int nTimeout /* = 1000*/, bool bExWrite /* = false*/)
{
	int ret = 0;
	string sKeySeed;

	if (bExWrite) {
		/*非标准modbus写文件记录号协议,默认使用非标准协议*/
		if ((ret = this->writeFileRecordEx(nFileNo, nRecordNo, pBuffer, nBufferSize))  < 0) {
			printf("write file record error:%d\n", ret);
			return ret;
		}
	} else {
		/*标准modbus写文件记录号协议*/
		if ((ret = this->writeFileRecord(nFileNo, nRecordNo, pBuffer, nBufferSize))  < 0) {
			printf("write file record error:%d\n", ret);
			return ret;
		}
	}

	return ret;
}

/* write file record ex
**/
int CWeconPLCModbusMasterCommu::writeFileRecordEx(int nFileNo/*in*/, int nRecordNo/*in*/,const unsigned char *pBuffer/*in*/, int nBufferSize/*in*/, int nTimeout/*in*/){
	unsigned char szSendBuffer[1024] = { 0 };
	unsigned char szRecvBuffer[1024] = { 0 };
	int nRtn,nDataSize,nRecvBytes;
	int nHopeSize = 0;
	const int nReferType = 0x08;

	/* package write file record */
	if ((nDataSize = m_modbusMaster.packageWriteFileRecordEx(nFileNo, nRecordNo, nReferType, pBuffer, nBufferSize, szSendBuffer, sizeof(szSendBuffer))) < 0){
		printf("modbus packageWriteFileRecordEx() error %d\n", nDataSize);
		return -1;
	}
	nHopeSize = m_modbusMaster.getRecvSize(FUNCTION_CODE_WRITE_FILERECORD, nBufferSize, 0x08);

	/* do commu */
	if ((nRecvBytes = this->doCommu(szSendBuffer, nDataSize, szRecvBuffer, sizeof(szRecvBuffer), nHopeSize, nTimeout)) <= 0){
		printf("modbus writeFileRecordEx doCommu error %d\n", nRecvBytes);
		return -2;
	}

	/* unpackage write file record ex*/
	if ((nRtn = m_modbusMaster.unpakageWriteFileRecordEx(szRecvBuffer, nRecvBytes, nFileNo, nRecordNo, nReferType)) < 0){
		printf("modbus unpakageWriteFileRecordEx() error %d\n", nRtn);
		return -3;
	}

	return 0;
}

/* read file
**/
int CWeconPLCModbusMasterCommu::readFileData(int nFileNo/*in*/, int nRecordNo/*in*/, int nFileSize, string& sData, int nTimeout /*= 1000*/)
{
	unsigned char szRecvBuffer[COMM_RW_MAX_SIZE] = {0};
	unsigned char szDecBuffer[COMM_RW_MAX_SIZE] = {0};
	int nRecvSize,  nReadCount, i, ret;

	if (!sData.empty()){
		sData.clear();
	}

	nReadCount = (nFileSize + COMM_RW_MAX_SIZE - 1) / COMM_RW_MAX_SIZE;
	sData.reserve(nReadCount * COMM_RW_MAX_SIZE);
	for (i = 0; i < nReadCount; ++i) {
		
		if(m_commFunc.on_commu_run) {
			ret =  m_commFunc.on_commu_run(i, nReadCount, m_commFunc.arg);
			if (ret < 0) {
				return ret;
			}
		}

		nRecvSize = this->readFileRecord(nFileNo, nRecordNo, COMM_RW_MAX_SIZE, szRecvBuffer, sizeof(szRecvBuffer), nTimeout);
		if (nRecvSize < 0)	{
			return nRecvSize;
		}

		ret = m_xor.DecData(szRecvBuffer, szDecBuffer, m_sKeySeed, nRecvSize);
		if (ret < 0){
			return ret;
		}

		sData.append((const char*)szDecBuffer, nRecvSize);	
		nRecordNo++;
	}

	return sData.size();
}

/* read wcp hash
**/
int CWeconPLCModbusMasterCommu::readWcpHash(string& sHash)
{
	vector<int> vecValue;
	if (this->readInterior(0x1002, 8, vecValue) < 0) 
	{
		return -1;
	}

	sHash.reserve(16);
	for (int i = 0; i < 8; i++)
	{
		sHash.append(1, vecValue[i]&0xFF);
		sHash.append(1, (vecValue[i] >> 8)&0xFF);
	}

	return 0;
}

int  CWeconPLCModbusMasterCommu::checkWcpHash(const string& sHash)
{
	int nSize, i;
	vector<int> vec;
	int value;
	const unsigned char* p = (const unsigned char*)sHash.data();
	vec.resize(sHash.size() / 2, 0);
	nSize = vec.size();
	for (i = 0; i < nSize; ++i)
	{
		value = (*(p + i * 2) << 8) | (*(p + i * 2 + 1));
		vec[i] = value;
	}

	return this->writeInterior(0x1002, vec);
}

int CWeconPLCModbusMasterCommu::writeMultiRegistersOnWork(int nAddress, const vector<int> &vecValue, int nCurProgess, int nTotoalProgress)
{
	int ret = 0;
	
	ret = CModbusMasterCommu::writeMultiRegisters(nAddress, vecValue);
	if (ret < 0) {
		return ret;
	}

	if (m_commFunc.on_commu_run) {
		ret = m_commFunc.on_commu_run(nCurProgess, nTotoalProgress, m_commFunc.arg);
		if (ret < 0) {
			return ret;
		}
	}

	if (m_commFunc.on_commu_end && nCurProgess == nTotoalProgress) {
		m_commFunc.on_commu_end(m_commFunc.arg);
	}

	return ret;
}

int CWeconPLCModbusMasterCommu::writeMultiCoilsOnWork(int nAddress, const vector<int> &vecValue, int nCurProgess, int nTotoalProgress)
{
	int ret;

	ret = CModbusMasterCommu::writeMultiCoils(nAddress, vecValue);
	if (ret < 0) {
		return ret;
	}

	if (m_commFunc.on_commu_run) {
		ret = m_commFunc.on_commu_run(nCurProgess, nTotoalProgress, m_commFunc.arg);
		if (ret < 0) {
			return ret;
		}
	}


	if (m_commFunc.on_commu_end && nCurProgess == nTotoalProgress) {
		m_commFunc.on_commu_end(m_commFunc.arg);
	}

	return ret;
}

int CWeconPLCModbusMasterCommu::readMultiRegistersOnWork(int nAddress, int nCount, vector<int> &vecValue, int nCurProgess, int nTotoalProgress)
{
	int ret = 0;
	
	ret = CModbusMasterCommu::readMultiRegisters(nAddress, nCount, vecValue);
	if (ret < 0) {
		return ret;
	}

	/*do progess*/
	if (m_commFunc.on_commu_run) {
		ret = m_commFunc.on_commu_run(nCurProgess, nTotoalProgress, m_commFunc.arg);
		if (ret < 0) {
			return ret;
		}
	}


	if (m_commFunc.on_commu_end && nCurProgess == nTotoalProgress) {
		m_commFunc.on_commu_end(m_commFunc.arg);
	}
	
	return ret;
}

int CWeconPLCModbusMasterCommu::readMultiCoilsOnWork(int nAddress, int nCount, vector<int> &vecValue, int nCurProgess, int nTotoalProgress)
{
	int ret = 0;

	ret = CModbusMasterCommu::readMultiCoils(nAddress, nCount, vecValue);
	if (ret < 0) {
		return ret;
	}

	if (m_commFunc.on_commu_run) {
		ret = m_commFunc.on_commu_run(nCurProgess, nTotoalProgress, m_commFunc.arg);
		if (ret < 0) {
			return ret;
		}
	}

	if (m_commFunc.on_commu_end && nCurProgess == nTotoalProgress) {
		m_commFunc.on_commu_end(m_commFunc.arg);
	}

	return ret;
}
void CWeconPLCModbusMasterCommu::setHandle(ONCOMMFUNC_T onCommuFunc)
{
	m_commFunc = onCommuFunc;
}

/* download file
**/
int CWeconPLCModbusMasterCommu::downloadFile(int nPreCode, PLC_FILE_NO fileNo, const list<FILE_ATTRIBUTE_T> &listFile, int nLimitSize, const string &sPassword, bool bExWrite, int nTimeout)
{
	string sData;
	CWeconPLCHeader header;
	int ret = 0;
	/*打包文件头*/
	ret = header.package(fileNo, listFile, nLimitSize, sData);
	if (ret < 0) {
		printf("downloadBinFile package()  error:%d\n", ret);
		goto __ERROR_EXIT;
	}

	/*临时解密*/
	COMM_FILE_BEGIN(DOWNLOAD_KEY);

	/*先发送一个子码,提示下位机准备下载文件*/
	ret = this->writeSubCode(nPreCode, nTimeout);
	if (ret < 0) {
		printf("downloadBinFile writeSubCode() prepareDownload  error:%d\n", ret);
		goto __ERROR_EXIT;
	}

	if (m_commFunc.on_commu_begin) {
		m_commFunc.on_commu_begin(m_commFunc.arg);
	}

	/*下载文件*/
	ret = this->writeFile(fileNo, (const unsigned char*)sData.c_str(), sData.size(), nTimeout, bExWrite);
	if (ret < 0) {
		printf("downloadBinFile writeFile()  error:%d\n", ret);
		goto __ERROR_EXIT;
	}
	
	/*下载完成*/
	ret = this->writeSubCode(FUNCTION_SUB_CODE_FINISH_DOWNLOAD, nTimeout);
	if (ret < 0) {
		printf("downloadBinFile writeSubCode() 0x1E error:%d\n", ret);
		goto __ERROR_EXIT;
	}

	if (m_commFunc.on_commu_end) {
		m_commFunc.on_commu_end(m_commFunc.arg);
	}

__ERROR_EXIT:
	/*解除临时解密*/
	COMM_FILE_END(DOWNLOAD_KEY);
	return ret;
}

/* upload file
**/
int CWeconPLCModbusMasterCommu::uploadFile(PLC_FILE_NO fileNo, list<string> &listFileData, const string &sPassword, int nTimeout)
{
	string sReplyData, sData;
	FILE_HEAD_T *pFileHead = NULL;
	CWeconPLCHeader header;
	int nRecord = 0, ret, nRemainSize;
	
	/*临时解密*/
	COMM_FILE_BEGIN(UPLOAD_KEY);
	if (!m_sKeySeed.empty()){
		m_sKeySeed.clear();
	}

	if (m_commFunc.on_commu_begin){
		m_commFunc.on_commu_begin(m_commFunc.arg);
	}
	/*获取密钥种子*/
	ret = this->getKeySeed(m_sKeySeed);
	if (ret < 0){
		goto __ERROR_EXIT;
	}
	
	do{
		/*第一次先读取HeadLen,再根据HeadLen读取文件头数据*/
		ret = this->readFileData(fileNo, nRecord, COMM_BUFFER_SIZE, sData, nTimeout);
		if (ret < 0){
			if (ret == MODBUS_NOT_HAVE_FILE) {
				/*默认上传空数据*/
				if (m_commFunc.on_commu_run) {
					m_commFunc.on_commu_run(1, 1, m_commFunc.arg);
				}
			}
			goto __ERROR_EXIT;
		}
		sReplyData.append(sData);
		nRemainSize = ((FILE_HEAD_T*)sReplyData.c_str())->HeadLen - COMM_BUFFER_SIZE * (++nRecord);
	}while(nRemainSize > 0);

	pFileHead = (FILE_HEAD_T*)sReplyData.c_str();
	/*检查文件头数据的hash*/
	ret = header.check(fileNo, (const char*)pFileHead, sReplyData.size());
	if (ret < 0){
		goto __ERROR_EXIT;
	}

	sData.clear();
	/*读取文件数据*/
	ret = this->readFileData(fileNo, nRecord, pFileHead->AllFileSize, sData, nTimeout);
	if (ret < 0){
		goto __ERROR_EXIT;
	}
	sReplyData.append(sData);
	/*解析并校验文件头，成功返回真实数据*/
	ret = header.unpackage(sReplyData, listFileData);
	if (ret < 0){
		goto __ERROR_EXIT;
	}

	if (m_commFunc.on_commu_end){
		m_commFunc.on_commu_end(m_commFunc.arg);
	}

__ERROR_EXIT:
	/*取消临时解密*/
	COMM_FILE_END(UPLOAD_KEY);
	return ret;
}

/* read module
**/
int CWeconPLCModbusMasterCommu::readModule(int nModuleNo/*in*/, int nAddress/*in*/, int nCount/*in*/, vector<int>& vecValue/*out*/, int nTimeout /*=1000*/)
{
	unsigned char szSendBuffer[1024] = { 0 };
	unsigned char szRecvBuffer[1024] = { 0 };
	int nRtn, nSendSize, nRecvBytes;
	int nHopeSize = 0;

	/*package send data*/
	if ((nSendSize = m_modbusMaster.packageReadModule(nModuleNo, nAddress, nCount, szSendBuffer, sizeof(szSendBuffer))) < 0)
	{
		printf("modbus readModule packageReadModule() error %d\n", nSendSize);
		return -1;
	}

	/*get hope recv size*/
	nHopeSize = m_modbusMaster.getSubRecvSize(WECON_PLC_FUNCTION_CODE_READ, FUNCTION_SUB_CODE_READ_MODULE, nCount);

	/*do commu*/
	if ((nRecvBytes = this->doCommu(szSendBuffer, nSendSize, szRecvBuffer, sizeof(szRecvBuffer), nHopeSize, nTimeout)) <= 0)
	{
		printf("modbus readModule doCommu() error %d\n", nRecvBytes);
		return -2;
	}

	/*unpackage recv data*/
	if ((nRtn = m_modbusMaster.unpackageReadModule(szRecvBuffer, nHopeSize, nModuleNo, nCount, vecValue)) < 0)
	{
		printf("modbus readModule unpackageReadModule() error %d\n", nRtn);
		return -3;
	}

	return 0;
}

/* write module
**/
int CWeconPLCModbusMasterCommu::writeModule(int nModuleNo, int nAddress, const vector<int>& vecValue, int nTimeout /*=1000*/)
{
	unsigned char szSendBuffer[1024] = { 0 };
	unsigned char szRecvBuffer[1024] = { 0 };
	unsigned char szFileBuffer[1024] = { 0 };
	int nRtn, nSendSize, nRecvBytes;
	int nHopeSize = 0, nCount;

	nCount = vecValue.size();
	/*package send data*/
	if ((nSendSize = m_modbusMaster.packageWriteModule(nModuleNo, nAddress, vecValue, szSendBuffer, sizeof(szSendBuffer))) < 0)
	{
		printf("modbus writeModule packageWriteModule() error %d\n", nSendSize);
		return -1;
	}

	/*get hope recv size*/
	nHopeSize = m_modbusMaster.getSubRecvSize(WECON_PLC_FUNCTION_CODE_WRITE, FUNCTION_SUB_CODE_WRITE_MODULE, nCount);

	/*do commu*/
	if ((nRecvBytes = this->doCommu(szSendBuffer, nSendSize, szRecvBuffer, sizeof(szRecvBuffer), nHopeSize, nTimeout)) <= 0)
	{
		printf("modbus writeModule doCommu() error %d\n", nRecvBytes);
		return -2;
	}

	/*unpackage recv data*/
	if ((nRtn = m_modbusMaster.unpackageWriteModule(szRecvBuffer, nHopeSize, nModuleNo, nAddress, nCount)) < 0)
	{
		printf("modbus writeModule unpackageReadModule() error %d\n", nRtn);
		return -3;
	}

	return 0;
}


void CWeconPLCModbusMasterCommu::makeupData(string& sInput, int demicals)
{
	int nRemain, nSize, nMakeup;

	nSize= sInput.size();
	nRemain = nSize % demicals;
	nMakeup = nRemain > 0 ? demicals - nRemain : 0;
	sInput.append(nMakeup, '\x00');
}