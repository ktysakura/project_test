#include "ModbusMasterCommu.h"
#include "file.h"
#include "md5.h"

/* construct
**/
CModbusMasterCommu::CModbusMasterCommu(){

}

/* destruct
**/
CModbusMasterCommu::~CModbusMasterCommu(){

}

/* set slave id
**/
int CModbusMasterCommu::setSlaveId(int nId){
    return m_modbusMaster.setSlaveId(nId);
}

int CModbusMasterCommu::setTransferMode(MODBUS_TRANSFER_T mode){
	return m_modbusMaster.setTransferMode(mode);
}
MODBUS_TRANSFER_T CModbusMasterCommu::getTransferMode(){
	return m_modbusMaster.getTransferMode();
}
int CModbusMasterCommu::readSingleCoil(int nAddress, bool& bOn, int nTimeout)
{
	int nRet = 0;
	vector<int> vecValue;

	if ((nRet = this->readCoils(nAddress, 1, vecValue, nTimeout)) < 0)
	{
		return nRet;
	}
	bOn = vecValue[0] > 0;

	return 0;
}
/* read Coils
*/
int CModbusMasterCommu::readCoils(int nAddress, int nCount, vector<int>& vecValue, int nTimeout){

    unsigned char szSendBuffer[1024] = { 0 };
    unsigned char szRecvBuffer[1024] = { 0 };
    int nRtn, nDataSize, nRecvBytes,nHopeSize = 0;

    /* package read registers */
    if ((nDataSize = m_modbusMaster.packageReadCoils(nAddress, nCount, szSendBuffer, sizeof(szSendBuffer))) < 0){
		printf("modbus packageReadCoils() doCommu error %d\n", nDataSize);
        return -1;
    }

    /* get hope size for rtu */
    if (MODBUS_TRANSFER_RTU == m_modbusMaster.getTransferMode()){
        nHopeSize = 5 + (nCount + 7) / 8;
    }

    /* do commu */
    if ((nRecvBytes = this->doCommu(szSendBuffer, nDataSize, szRecvBuffer, sizeof(szRecvBuffer), nHopeSize, nTimeout)) <= 0){
//        stp_mem(szSendBuffer, nDataSize);
        printf("modbus readCoils doCommu error %d\n", nRecvBytes);
        return -2;
    }

    /* unpackage read registers */
    if ((nRtn = m_modbusMaster.unpackageReadCoils(szRecvBuffer, nRecvBytes, nCount, vecValue)) < 0){
        printf("modbus unpackageReadCoils() error %d\n", nRtn);
        return -3;
    }

    /* compare register count */
    if (nCount != (int)vecValue.size()){
        return -4;
    }

    return 0;
}
/* read Coils
**/
int CModbusMasterCommu::readSingleRegister(int nAddress, int& nValue, int nTimeout /*= 1000*/)
{
	int nRet = 0;
	vector<int> vecValue;
	
	if ((nRet = this->readRegisters(nAddress, 1, vecValue, nTimeout)) < 0)
	{
		return nRet;
	}
	nValue = vecValue[0];

	return 0;

}
/* read registers
**/
int CModbusMasterCommu::readRegisters(int nAddress, int nCount, vector<int>& vecValue,int nTimeout){

	unsigned char szSendBuffer[1024] = { 0 };
	unsigned char szRecvBuffer[1024] = { 0 };
	int nRtn, nDataSize, nRecvBytes,nHopeSize = 0;

	/* package read registers */
	if ((nDataSize = m_modbusMaster.packageReadRegisters(nAddress, nCount, szSendBuffer, sizeof(szSendBuffer))) < 0){
		printf("modbus packageReadRegisters() error %d\n", nDataSize);
		return -1;
	}

	/* get hope size for rtu */
	if (MODBUS_TRANSFER_RTU == m_modbusMaster.getTransferMode()){
		nHopeSize = 5 + nCount * 2;
	}
	
	/* do commu */
    if ((nRecvBytes = this->doCommu(szSendBuffer, nDataSize, szRecvBuffer, sizeof(szRecvBuffer), nHopeSize, nTimeout)) <= 0){
		printf("modbus readRegisters doCommu error %d\n", nRecvBytes);
		return -2;
	}

	/* unpackage read registers */
	if ((nRtn = m_modbusMaster.unpackageReadRegisters(szRecvBuffer, nRecvBytes, vecValue)) < 0){
		printf("modbus unpackageReadRegisters() error %d\n", nRtn);
		return -3;
	}

	/* compare register count */
	if (nCount != (int)vecValue.size()){
		return -4;
	}

	return 0;
}

/* read coils
**/
int CModbusMasterCommu::readMultiCoils(int nAddress, int nCount, vector<int>& vecValue, int nTimeout)
{
	int i, nSize, nOffset, nRet, nReadCount;
	vector<int> vecLen, vecReadValue;

	onceAllocate(nCount, READ_COILS_MAX_COUNT, vecLen);
	nOffset = 0;
	nSize = vecLen.size();
	vecValue.clear();
	vecValue.reserve(nCount);
	for (i = 0; i < nSize; ++i) {
		nReadCount = vecLen[i];
		if ((nRet = this->readCoils(nAddress + nOffset, nReadCount, vecReadValue, nTimeout)) < 0) {
			return -1;
		}
		copy(vecReadValue.begin(), vecReadValue.end(), back_inserter(vecValue));
		nOffset += nReadCount;
	}

	return 0;
}
/* read multi registers > 0x007D 125
**/
int CModbusMasterCommu::readMultiRegisters(int nAddress, int nCount, vector<int>& vecValue, int nTimeout /*= 1000*/){
	int i, nSize, nOffset, nRet, nReadCount;
	vector<int> vecLen, vecReadValue;

	onceAllocate(nCount, READ_REGISTERS_MAX_COUNT, vecLen);
	nOffset = 0;
	nSize = vecLen.size();
	vecValue.clear();
	vecValue.reserve(nCount);
	for (i = 0; i < nSize; ++i) {
		nReadCount = vecLen[i];
		if ((nRet = this->readRegisters(nAddress + nOffset, nReadCount, vecReadValue, nTimeout)) < 0) {
			return -1;
		}
		copy(vecReadValue.begin(), vecReadValue.end(), back_inserter(vecValue));
		nOffset += nReadCount;
	}

	return 0;
	
}

/* write single coils
*/
int CModbusMasterCommu::writeSingleCoil(int nAddress, int nValue, int nTimeout){

    return this->writeCoils(nAddress, vector<int>(1, nValue), nTimeout);
}



/* write Coils
*/
int CModbusMasterCommu::writeCoils(int nAddress, const vector<int>& vecValue, int nTimeout){
    unsigned char szSendBuffer[1024] = { 0 };
    unsigned char szRecvBuffer[1024] = { 0 };
    int nRtn,nDataSize,nRecvBytes;
    int nStartAddress, nRegisterCount, nHopeSize = 0;

    /* package write coils */
    if ((nDataSize = m_modbusMaster.packageWriteCoils(nAddress, vecValue, szSendBuffer, sizeof(szSendBuffer))) < 0){
        printf("modbus packageWriteCoils() error %d\n", nDataSize);
        return -1;
    }

    /* get hope size for rtu */
    if (MODBUS_TRANSFER_RTU == m_modbusMaster.getTransferMode()){
        nHopeSize = 8;
    }

    /* do commu */
    if ((nRecvBytes = this->doCommu(szSendBuffer, nDataSize, szRecvBuffer, sizeof(szRecvBuffer), nHopeSize, nTimeout)) <= 0){
		printf("modbus writeCoils doCommu error %d\n", nRecvBytes);       
		return -2;
    }

    /* unpackage write coils */
    if ((nRtn = m_modbusMaster.unpackageWriteCoils(szRecvBuffer, nRecvBytes, nStartAddress, nRegisterCount)) < 0){
        printf("modbus unpackageWriteCoils() error %d\n", nRtn);
        return -3;
    }

    /* compare address and register count */
    if ((nAddress != nStartAddress) || ((int)vecValue.size() != nRegisterCount)){
        printf("diff address(%d,%d) or diff register count(%d,%d)\n", nAddress, nStartAddress, (int)vecValue.size(), nRegisterCount);
        return -4;
    }

    return 0;
}

/* write single registers
*/
int CModbusMasterCommu::writeSingleRegister(int nAddress,  int nValue, int nTimeout){

	return this->writeRegisters(nAddress, vector<int>(1, nValue), nTimeout);
}
/* write multi Coils
*/
int CModbusMasterCommu::writeMultiCoils(int nAddress, const vector<int>& vecValue, int nTimeout) {
	int nOffset, ret, nWriteCount, i;
	vector<int> vecLen, vecWriteValue;

	nOffset = 0;
	onceAllocate(vecValue.size(), WRITE_COILS_MAX_COUNT, vecLen);
	for (i = 0; i < (int)vecLen.size(); ++i) {
		nWriteCount = vecLen[i];
		vecWriteValue.assign(vecValue.begin() + nOffset, vecValue.begin() + nOffset + nWriteCount);
		ret = this->writeCoils(nAddress + nOffset, vecWriteValue, nTimeout);
		if(ret < 0) {
			return -1;
		}
		nOffset += nWriteCount;
	}

	return 0;
}
/* write registers
**/
int CModbusMasterCommu::writeRegisters(int nAddress, const vector<int>& vecValue,int nTimeout){

    unsigned char szSendBuffer[1024] = { 0 };
    unsigned char szRecvBuffer[1024] = { 0 };
    int nRtn,nDataSize,nRecvBytes;
    int nStartAddress, nRegisterCount, nHopeSize = 0;

    /* package write registers */
    if ((nDataSize = m_modbusMaster.packageWriteRegisters(nAddress, vecValue, szSendBuffer, sizeof(szSendBuffer))) < 0){
        printf("modbus packageWriteRegisters() error %d\n", nDataSize);
        return -1;
    }

    /* get hope size for rtu */
    if (MODBUS_TRANSFER_RTU == m_modbusMaster.getTransferMode()){
        nHopeSize = 8;
    }

    /* do commu */
    if ((nRecvBytes = this->doCommu(szSendBuffer, nDataSize, szRecvBuffer, sizeof(szRecvBuffer), nHopeSize, nTimeout)) <= 0){
		 printf("modbus writeRegisters doCommu error %d\n", nRecvBytes);
        return -2;
    }

    /* unpackage write registers */
    if ((nRtn = m_modbusMaster.unpackageWriteRegisters(szRecvBuffer, nRecvBytes, nStartAddress, nRegisterCount)) < 0){
        printf("modbus unpackageWriteRegisters() error %d\n", nRtn);
        return -3;
    }

    /* compare address and register count */
    if ((nAddress != nStartAddress) || ((int)vecValue.size() != nRegisterCount)){
        printf("diff address(%d,%d) or diff register count(%d,%d)\n", nAddress, nStartAddress, (int)vecValue.size(), nRegisterCount);
        return -4;
    }

    return 0;
}

/* write multi registers > 0x007B
**/
int CModbusMasterCommu::writeMultiRegisters(int nAddress, const vector<int>& vecValue, int nTimeout /*= 1000*/){
	int nOffset, ret, nWriteCount, i;
	vector<int> vecLen, vecWriteValue;
	
	nOffset = 0;
	onceAllocate(vecValue.size(), WRITE_REGISTERS_MAX_COUNT, vecLen);
	for (i = 0; i < (int)vecLen.size(); ++i) {
		nWriteCount = vecLen[i];
		vecWriteValue.assign(vecValue.begin() + nOffset, vecValue.begin() + nOffset + nWriteCount);
		ret = this->writeRegisters(nAddress + nOffset, vecWriteValue, nTimeout);
		if(ret < 0) {
			return -1;
		}
		nOffset += nWriteCount;
	}

	return 0;
}
/* write file record
**/
int CModbusMasterCommu::writeFileRecord(int nFileNo/*in*/, int nRecordNo/*in*/, unsigned char *pBuffer/*in*/, int nBufferSize/*in*/, int nTimeout/*in*/){
	unsigned char szSendBuffer[1024] = { 0 };
	unsigned char szRecvBuffer[1024] = { 0 };
	unsigned char szFileBuffer[1024] = { 0 };
	int nRtn,nDataSize,nRecvBytes, nFileBytes;
	int nHopeSize = 0;
	const int nReferType = 0x06;

	/* package write file record */
	if ((nDataSize = m_modbusMaster.packageWriteFileRecord(nFileNo, nRecordNo, nReferType, pBuffer, nBufferSize, szSendBuffer, sizeof(szSendBuffer))) < 0){
		printf("modbus packageWriteFileRecord() error %d\n", nDataSize);
		return -1;
	}
	
	nHopeSize = m_modbusMaster.getRecvSize(FUNCTION_CODE_WRITE_FILERECORD, nBufferSize, 0x06);
	/* do commu */
	if ((nRecvBytes = this->doCommu(szSendBuffer, nDataSize, szRecvBuffer, sizeof(szRecvBuffer), nHopeSize, nTimeout)) <= 0)
	{
		printf("modbus writeFileRecord doCommu error %d\n", nRecvBytes);
		return -2;
	}

	/* unpackage write file record */
	if ((nRtn = m_modbusMaster.unpakageWriteFileRecord(szRecvBuffer, nRecvBytes, nFileNo, nRecordNo, nReferType, szFileBuffer, sizeof(szFileBuffer), nFileBytes)) < 0){
		printf("modbus unpakageWriteFileRecord() error %d\n", nRtn);
		return -3;
	}

	if ((nFileBytes != nBufferSize) && (memcmp(pBuffer, szFileBuffer, nFileBytes) != 0))
	{
		printf("modbus writeFileRecord() diff FileBytes(%d, %d) or mem\n", nFileBytes, nBufferSize);
	}
	 return 0;
}


/* read file record
**/
int CModbusMasterCommu::readFileRecord(int nFileNo/*in*/, int nRecordNo/*in*/, int nRecordSize/*in*/, unsigned char* pFileBuffer/*out*/, int nMaxBufferSize, int nTimeout/*in*/)
{
	unsigned char szSendBuffer[1024] = { 0 };
	unsigned char szRecvBuffer[1024] = { 0 };
	unsigned char szFileBuffer[1024] = { 0 };
	int nRtn,nDataSize,nRecvBytes, nFileBytes;
	int nHopeSize = 0;
	
	/* package read file record */
	if ((nDataSize = m_modbusMaster.packageReadFileRecord(nFileNo, nRecordNo, nRecordSize, szSendBuffer, sizeof(szSendBuffer))) < 0){
		printf("modbus packageWriteFileRecord() error %d\n", nDataSize);
		return -1;
	}
	
	nHopeSize = m_modbusMaster.getRecvSize(FUNCTION_CODE_READ_FILERECORD, nRecordSize);
	/* do commu */
	if ((nRecvBytes = this->doCommu(szSendBuffer, nDataSize, szRecvBuffer, sizeof(szRecvBuffer), nHopeSize, nTimeout)) <= 0)
	{
		printf("modbus readFileRecord doCommu error %d\n", nRecvBytes);
		return -2;
	}

	/* unpackage read file record */
	if ((nRtn = m_modbusMaster.unpakageReadFileRecord(szRecvBuffer, nRecvBytes, pFileBuffer, nMaxBufferSize, nFileBytes)) < 0)
	{
		printf("modbus unpakageReadFileRecord() error %d\n", nRtn);
		return nRtn;
	}

	if (nFileBytes != nRecordSize)
	{
		printf("modbus readFileRecord() diff fileRecordSize(%d,%d)\n", nFileBytes, nRecordSize);
		return -4;
	}

	return nFileBytes;
}
int CModbusMasterCommu::onceAllocate(int nMultiCount, int nMaxOnceCount, vector<int> &vecSendCount)
{
	int nRemain = 0;
	vecSendCount.assign((nMultiCount + nMaxOnceCount - 1) / nMaxOnceCount, nMaxOnceCount);
	if ((nRemain = nMultiCount % nMaxOnceCount) != 0) {
		vecSendCount[vecSendCount.size() - 1] = nRemain;
	}
	return 0;
}
/* find reply package
**/
int CModbusMasterCommu::findReplyPackage(unsigned char *pData, int nDataSize, int& nPackageSize){
	return m_modbusMaster.findReplyPackage(pData, nDataSize, nPackageSize);
}

int CModbusMasterCommu::doCommu(unsigned char *pSendData/*in*/, int nDataSize/*in*/, unsigned char *pRecvBuffer/*out*/, int nBufferSize/*in*/ ,int nHopeSize/*in*/, int nTimeout/*in*/)
{
	int nReadTotal = 0;
	CAutoLock locker(&m_commlock);
//	printf("lock addr:%x\n", &m_commlock);
	nReadTotal = this->sendAndRecv(pSendData, nDataSize, pRecvBuffer, nBufferSize, nHopeSize, nTimeout);
	if (nReadTotal != nHopeSize)
	{
		this->flush();
		printf("doCommu() nReadSize (%d)!= nHopeSize(%d) error(%d):\n", nReadTotal, nHopeSize, modbus_status_frame_not_equal);
		return modbus_status_frame_not_equal;
	} 

	return nReadTotal;

}