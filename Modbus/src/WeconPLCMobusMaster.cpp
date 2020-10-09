#include "WeconPLCMobusMaster.h"

CWeconPLCMobusMaster::CWeconPLCMobusMaster() 
{
	m_pModbusPdu = &m_weconPLCModbusPdu;
}

CWeconPLCMobusMaster::~CWeconPLCMobusMaster(void)
{

}

int CWeconPLCMobusMaster::packWriteBitInRegister(int nAddress, int bit, bool bOn, unsigned char *pBuffer, int nBufferSize)
{
	int nPduSize;
	unsigned char szBuffer[512] = { 0 };
	CAutoLock locker(&m_mutex);

	/* make pdu data */
	nPduSize = m_weconPLCModbusPdu.packWriteBitInRegisterRequest(nAddress, bit, bOn, szBuffer, sizeof(szBuffer));
	if (nPduSize < 0) {
		return (nPduSize - 32);
	}

	return this->packageRequest(szBuffer,  nPduSize, pBuffer, nBufferSize);
}
int CWeconPLCMobusMaster::unpackWriteBitInRegister(unsigned char *pData/*in*/, int nDataSize/*in*/, int nAddress, int bit, bool bOn)
{
	int nPduSize, nFuncCode;

	CAutoLock locker(&m_mutex);
	
	/* check reply package */
	if (this->checkReplyPackage(pData, nDataSize, nFuncCode, nPduSize) < 0) {
		return -1;
	}
	
	/* check func code */
	if (nFuncCode != WECON_PLC_FUNCTION_CODE_WRITE) {
		return -2;
	}

	/*get reply data*/
	if (m_weconPLCModbusPdu.dataWithWriteBitInRegisterReply(pData + m_nOffset, nDataSize - 1, nAddress, bit, bOn) < 0) {
		return -3;
	}

	return 0;
}
/* package read module
**/
int CWeconPLCMobusMaster::packageReadModule(int nModuleNo, int nAddress, int nCount, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/)
{
	int nPduSize;
	unsigned char szBuffer[512] = { 0 };
	CAutoLock locker(&m_mutex);

	/* make pdu data */
	if ((nPduSize = m_weconPLCModbusPdu.packageReadModuleRequest(nModuleNo, nAddress, nCount, szBuffer, sizeof(szBuffer))) < 0)
	{
		return (nPduSize - 32);
	}

	return this->packageRequest(szBuffer,  nPduSize, pBuffer, nBufferSize);
}
/* package read module
**/
int  CWeconPLCMobusMaster::unpackageReadModule(unsigned char *pData/*in*/, int nDataSize/*in*/, int nModuleNo/*in*/, int nCount, vector<int>& vecValue/*out*/)
{
	CAutoLock locker(&m_mutex);
	int nPduSize, nFuncCode;
	
	/* check reply package */
	if (this->checkReplyPackage(pData, nDataSize, nFuncCode, nPduSize) < 0)
	{
		return -1;
	}

	/* check func code */
	if (nFuncCode != WECON_PLC_FUNCTION_CODE_READ)
	{
		return -2;
	}

	/*get reply data*/
	if (m_weconPLCModbusPdu.dataReadModuleReply(pData + m_nOffset, nDataSize - 1, nModuleNo, nCount, vecValue) < 0)
	{
		return -3;
	}

	return 0;
}
/* package write module
**/
int CWeconPLCMobusMaster::packageWriteModule(int nModuleNo/*in*/, int nAddress/*in*/, const vector<int>& vecValue/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/)
{
	int nPduSize;
	unsigned char szBuffer[512] = { 0 };
	CAutoLock locker(&m_mutex);

	/* make pdu data */
	if ((nPduSize = m_weconPLCModbusPdu.packageWriteModuleRequest(nModuleNo, nAddress, vecValue, szBuffer, sizeof(szBuffer))) < 0)
	{
		return (nPduSize - 32);
	}

	return this->packageRequest(szBuffer,  nPduSize, pBuffer, nBufferSize);
}
/* unpackage write module
**/
int CWeconPLCMobusMaster::unpackageWriteModule(unsigned char *pData/*in*/, int nDataSize/*in*/, int nModuleNo, int nAddress, int nCount)
{
	int nFuncCode, nPduSize;
	CAutoLock locker(&m_mutex);

	/*check reply package*/
	if (this->checkReplyPackage(pData, nDataSize, nFuncCode, nPduSize) < 0)
	{
		return -1;
	}
	
	/*check func code*/
	if (nFuncCode != WECON_PLC_FUNCTION_CODE_WRITE)
	{
		return -2;
	}
	
	/*check write module reply*/
	if (m_weconPLCModbusPdu.dataWriteModuleReply(pData + m_nOffset, nDataSize - 1, nModuleNo, nAddress, nCount) < 0)
	{
		return -3;
	}

	return 0;
}

/* package sub code
**/
int CWeconPLCMobusMaster::packageMutliSubCode(int nSubCode/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/)
{
	int nPduSize;
	unsigned char szBuffer[1024] = { 0 };
	CAutoLock locker(&m_mutex);

	/* make pdu data */
	if ((nPduSize = m_weconPLCModbusPdu.packageWriteSubCodeRequest(nSubCode, szBuffer, sizeof(szBuffer))) <= 0)
	{
		return (nPduSize - 32);
	}

	return this->packageRequest(szBuffer, nPduSize, pBuffer, nBufferSize);
}

/* unpackage sub code
**/
int CWeconPLCMobusMaster::unpackageMutliSubCode(unsigned char *pData/*in*/, int nDataSize/*in*/, int& nSubCode)
{

	int nPduSize, nFuncCode;
	CAutoLock locker(&m_mutex);

	/* check reply package */
	if (checkReplyPackage(pData, nDataSize, nFuncCode, nPduSize) < 0)
	{
		return -1;
	}

	/* check func code */
	if (WECON_PLC_FUNCTION_CODE_WRITE_SUB_CODE != nFuncCode){
		return -2;
	}

	/* get payload data */
	if (m_weconPLCModbusPdu.dataWriteSubCodeReply(pData + m_nOffset, nPduSize - 1, nSubCode) < 0)
	{
		return -3;
	}

	return 0;
}

/* package file recordex
**/
int CWeconPLCMobusMaster::packageWriteFileRecordEx(int nFileNo/*in*/, int nRecordNo/*in*/, int nReferType/*in*/, const unsigned char *pData/*in*/, int nDataSize/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/)
{
	return this->packageWriteFileRecord(nFileNo, nRecordNo, nReferType, pData, nDataSize, pBuffer, nBufferSize);
}

/* unpackage file recordex
**/
int CWeconPLCMobusMaster::unpakageWriteFileRecordEx(unsigned char *pData/*in*/, int nDataSize/*in*/, int nFileNo/*in*/, int nRecordNo/*in*/, int nReferType/*in*/)
{
	CAutoLock locker(&m_mutex);
	int nPduSize, nFuncCode;

	/* check reply package */
	if (checkReplyPackage(pData, nDataSize, nFuncCode, nPduSize) < 0)
	{
		return -1;
	}

	/* check func code */
	if (FUNCTION_CODE_WRITE_FILERECORD != nFuncCode)
	{
		return -2;
	}

	/* get payload data */
	if (m_weconPLCModbusPdu.dataWriteFileRecordReplyEx(pData + m_nOffset, nPduSize - 1, nFileNo, nRecordNo, nReferType) < 0)
	{
		return -3;
	}

	return 0;
}
/* package read plc info
**/
int CWeconPLCMobusMaster::packageOperatePassword(int nSubCode, unsigned char cKeyType, const string& sPassword, const string& sKeySeed,  unsigned char* pBuffer, int nBufferSize)
{
	int nPduSize;
	unsigned char szBuffer[1024] = { 0 };
	CAutoLock locker(&m_mutex);

	/* make pdu data */
	nPduSize = m_weconPLCModbusPdu.packageOperatePasswordRequest(nSubCode, cKeyType, sPassword, sKeySeed, szBuffer, sizeof(szBuffer));
	if (nPduSize < 0)
	{
		return -1;
	}

	return this->packageRequest(szBuffer, nPduSize, pBuffer, nBufferSize);
}

int CWeconPLCMobusMaster::unpackageOperatePassword(unsigned char *pData/*in*/, int nDataSize/*in*/,int nSubCode, int &nCode)
{
	int nPduSize, nFunCode;
	CAutoLock locker(&m_mutex);

	/* check reply package */
	if (this->checkReplyPackage(pData, nDataSize, nFunCode, nPduSize) < 0)
	{
		return -1;
	}

	/* check func code */
	if (nFunCode != WECON_PLC_FUNCTION_CODE_WRITE)
	{
		return -2;
	}

	/* get payload data */
	if (m_weconPLCModbusPdu.dataWithOperatePasswordReply(pData + m_nOffset, nPduSize - 1, nSubCode, nCode) < 0)
	{
		return -3;
	}

	return 0;
}

/* package read plc info
**/
int CWeconPLCMobusMaster::packageReadPLCInfo(int nSubCode, int nIndex, int nCount, unsigned char* pBuffer, int nBufferSize)
{
	int nPduSize;
	unsigned char szBuffer[1024] = { 0 };
	CAutoLock locker(&m_mutex);
	
	nPduSize = m_weconPLCModbusPdu.packageReadPLCInfoRequest(nSubCode, nIndex, nCount, szBuffer, sizeof(szBuffer));
	if (nPduSize < 0)
	{
		return -1;
	}

	return this->packageRequest(szBuffer, nPduSize, pBuffer, nBufferSize);
}

/* unpackage read plc info
**/
int CWeconPLCMobusMaster::unpackageReadPLCInfo(unsigned char *pData/*in*/, int nDataSize/*in*/, int nSubCode, int nCount, string& sResponse)
{
	int nPduSize, nFunCode;
	CAutoLock locker(&m_mutex);

	/* check reply package */
	if (this->checkReplyPackage(pData, nDataSize, nFunCode, nPduSize) < 0)
	{
		return -1;
	}

	/* check func code */
	if (nFunCode != WECON_PLC_FUNCTION_CODE_READ)
	{
		return -2;
	}

	/* get payload data */
	if (m_weconPLCModbusPdu.dataWithReadPLCInfoReply(pData + m_nOffset, nPduSize - 1, nSubCode, nCount, sResponse) < 0)
	{
		return -3;
	}

	return 0;
}

/* package remove dec temporary
**/
int CWeconPLCMobusMaster::packageRemoveDecTemporary(const vector<char>& vecKey, unsigned char* pBuffer, int nBufferSize)
{
	int nPduSize;
	unsigned char szBuffer[1024] = { 0 };
	CAutoLock locker(&m_mutex);

	nPduSize = m_weconPLCModbusPdu.packageRemoveDecTemporaryRequest(vecKey, szBuffer, sizeof(szBuffer));
	if (nPduSize < 0)
	{
		return -1;
	}

	return this->packageRequest(szBuffer, nPduSize, pBuffer, nBufferSize);
}

/* unpackage remove dec temporary
**/
int CWeconPLCMobusMaster::unpackageRemoveDecTemporary(unsigned char* pData, int nDataSize, const vector<char> &vecKey)
{
	int nPduSize, nFunCode;
	CAutoLock locker(&m_mutex);

	if (checkReplyPackage(pData, nDataSize, nFunCode, nPduSize) < 0)
	{
		return -1;
	}
	
	if (nFunCode != WECON_PLC_FUNCTION_CODE_WRITE)
	{
		return -2;
	}

	if (m_weconPLCModbusPdu.dataWithRemoveDecTemporaryReply(pData + m_nOffset, nPduSize - 1, vecKey) < 0)
	{
		return -3;
	}

	return 0;
}

/* package write interior
**/
int CWeconPLCMobusMaster::packageWriteInterior(int nAddress, const vector<int>& vecValue, unsigned char* pBuffer, int nBufferSize)
{
	int nPduSize;
	unsigned char szBuffer[1024] = { 0 };
	CAutoLock locker(&m_mutex);

	/* make pdu data */
	if ((nPduSize = m_weconPLCModbusPdu.packageWriteInteriorRequest(nAddress, vecValue, szBuffer, sizeof(szBuffer))) < 0)
	{
		return (nPduSize - 32);
	}

	return this->packageRequest(szBuffer,  nPduSize, pBuffer, nBufferSize);
}

/* unpackage write interior
**/
int CWeconPLCMobusMaster::unpackageWriteInterior(unsigned char *pData/*in*/, int nDataSize/*in*/, int nAddress, int nCount)
{
	int nPduSize, nFuncCode;
	CAutoLock locker(&m_mutex);
	

	/* check reply package */
	if (checkReplyPackage(pData, nDataSize, nFuncCode, nPduSize) < 0)
	{
		return -1;
	}

	/* check func code */
	if (WECON_PLC_FUNCTION_CODE_WRITE != nFuncCode)
	{
		return -2;
	}

	/* get payload data */
	if (m_weconPLCModbusPdu.dataWithWriteInteriorReply(pData + m_nOffset, nPduSize - 1, nAddress, nCount) < 0)
	{
		return -3;
	}

	return 0;
}

/* package read interior
**/
int CWeconPLCMobusMaster::packageReadInterior(int nAddress, int nCount, unsigned char* pBuffer, int nBufferSize)
{
	int nPduSize;
	unsigned char szBuffer[1024] = { 0 };
	CAutoLock locker(&m_mutex);

	nPduSize = m_weconPLCModbusPdu.packageReadInteriorRequest(nAddress, nCount, szBuffer, sizeof(szBuffer));
	if (nPduSize < 0)
	{
		return (nPduSize - 32);
	}

	return this->packageRequest(szBuffer,  nPduSize, pBuffer, nBufferSize);
}

/* unpackage read interior
**/
int CWeconPLCMobusMaster::unpackageReadInterior(unsigned char *pData/*in*/, int nDataSize/*in*/, int nAddress, int nCount, vector<int>& vecValue)
{
	int nPduSize, nFuncCode;
	CAutoLock locker(&m_mutex);


	/* check reply package */
	if (checkReplyPackage(pData, nDataSize, nFuncCode, nPduSize) < 0)
	{
		return -1;
	}

	/* check func code */
	if (WECON_PLC_FUNCTION_CODE_WRITE != nFuncCode)
	{
		return -2;
	}

	/* get payload data */
	if (m_weconPLCModbusPdu.dataWithReadInteriorReply(pData + m_nOffset, nPduSize - 1, nAddress, nCount, vecValue) < 0)
	{
		return -3;
	}

	return 0;
}

/* get recv size
**/
int CWeconPLCMobusMaster::getRecvSize(int nCode, int nCount, int nSubCode /*= -1*/)
{
	int nSize = 0;
	int nOffset = (m_transferMode == MODBUS_TRANSFER_RTU) ? 3 : 7 ;
	
	switch(nCode)
	{
		case FUNCTION_CODE_READ_FILERECORD:{
			nSize = 4 + nOffset + nCount;
		}
		break;
		case FUNCTION_CODE_WRITE_FILERECORD:{
			if (nSubCode == 0x06)
			{
				nSize = 9 + nOffset + nCount;
			}
			else if (nSubCode == 0x08)
			{
				nSize = 9 + nOffset;
			}	
		}
		break;
		case FUNCTION_CODE_READ_REGISTERS:{

			nSize = 2 + nOffset + (nCount << 1);
		}
		break;
		case FUNCTION_CODE_READ_COILS:{
			nSize = 2 + nOffset + ((nCount + 7) >> 3);
		} 
		break;
		case FUNCTION_CODE_WRITE_SINGLE_REGISTER:
		case FUNCTION_CODE_WRITE_COILS:
		case FUNCTION_CODE_WRITE_SINGLE_COIL:
		case FUNCTION_CODE_WRITE_REGISTERS:{
			nSize = 5 + nOffset;
		}
		break;
		default:
		break;
	}

	return nSize;
}

/* get sub recv size
**/
int CWeconPLCMobusMaster::getSubRecvSize(int nCode, int nSubCode, int nCount /*= 0*/)
{
	int nSize = 0;
	int nOffset = (m_transferMode == MODBUS_TRANSFER_RTU) ? 3 : 7 ;

	switch(nCode)
	{
	case WECON_PLC_FUNCTION_CODE_WRITE_SUB_CODE:
		nSize = 4 + nOffset;
	break;
	case WECON_PLC_FUNCTION_CODE_READ:
		switch(nSubCode)
		{
		case FUNCTION_SUB_CODE_READ_MODULE:
			nSize = 5 + nOffset + nCount * 2;
		break;
		case FUNCTION_SUB_CODE_RW_INTERIOR:
			nSize = 6 + nOffset + nCount * 2;
		break;
		case FUNCTION_SUB_CODE_READ_ENCRYPT_MESSAGE:
		case FUNCTION_SUB_CODE_READ_RANDOM_SEED:
			nSize = 4 + nCount * 2 + nOffset;
		break;	
		}
	break;	
	case WECON_PLC_FUNCTION_CODE_WRITE:
		switch(nSubCode)
		{
		case FUNCTION_SUB_CODE_RW_INTERIOR:
		case FUNCTION_SUB_CODE_WRITE_MODULE:
		case FUNCTION_SUB_CODE_WRITE_BITINREGISTER:
			nSize = 7 + nOffset;
		break;
		case FUNCTION_SUB_CODE_WRITE_PASSWORD:
		case FUNCTION_SUB_CODE_TEMPORARY_DECRYPT:
		case FUNCTION_SUB_CODE_VERIFY_PASSWORD:
		case FUNCTION_SUB_CODE_CHECK_PASSWORD:
		case FUNCTION_SUB_CODE_REMOVE_PASSWORD:
			nSize = 5 + nOffset;
		break;
		case FUNCTION_SUB_CODE_REMOVE_TEMPORARY_DECRYPT:
			nSize = 6 + nCount + nOffset;
		break;
		}
	break;
	}

	return nSize;
}