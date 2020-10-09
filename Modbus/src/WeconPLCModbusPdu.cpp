#include "WeconPLCModbusPdu.h"
#include "XorEncAndDec.h"
namespace cbl{

	CWeconPLCModbusPdu::CWeconPLCModbusPdu(void)
	{
	}

	CWeconPLCModbusPdu::~CWeconPLCModbusPdu(void)
	{

	}

	int CWeconPLCModbusPdu::packWriteBitInRegisterRequest(int nAddress, int bit, bool bOn, unsigned char *pBuffer, int nBufferSize)
	{
		int nPduSize = 0;
		unsigned char *pHead = NULL;
		unsigned char szBuffer[512] = { 0 };    

		if ((nAddress < 0)
			|| (nAddress >= 0x10000)
			|| (bit < 0)
			|| (bit > 15)) {
			return -1;
		}

		pHead = szBuffer;
		*pHead++ = WECON_PLC_FUNCTION_CODE_WRITE;
		*pHead++ = 0x07;
		*pHead++ = FUNCTION_SUB_CODE_WRITE_BITINREGISTER;
		*pHead++ = HIBYTE(nAddress);
		*pHead++ = LOBYTE(nAddress);
		*pHead++ = LOBYTE(bit);
		*pHead++ = bOn;

		nPduSize = (int)(pHead - szBuffer);
		if (nPduSize > nBufferSize) {
			return -2;
		}
		memcpy(pBuffer, szBuffer, nPduSize);
		return nPduSize;
	}

	int CWeconPLCModbusPdu::dataWithWriteBitInRegisterReply(unsigned char *pData, int nDataSize, int nAddress, int bit, bool bOn)
	{
		unsigned char *pHead = NULL;
		int nPduSize = 0;
		int i = 0, nValue = 0;
		unsigned char bRecvOn;
		int nRecvSubCode, nRecvAddress, nRecvBit;
		if ((NULL == pData)
			|| (nDataSize < 6)
			)
		{
			return -1;
		}

		pHead = pData;
		nPduSize = *pHead++;
		nRecvSubCode = *pHead++;
		nRecvAddress =  getBigendianValueU16(pHead);
		pHead += 2;
		nRecvBit = *pHead++;
		bRecvOn = *pHead++;
		if ((nPduSize != 0x07)
			||(nRecvSubCode != FUNCTION_SUB_CODE_WRITE_BITINREGISTER)
			|| (nRecvAddress != nAddress)
			|| (nRecvBit != bit)
			|| (bRecvOn != (unsigned char)bOn)
			) 
		{
				return -2;
		}

		return 0;
	}
	/* package read module request 
	**/
	int CWeconPLCModbusPdu::packageReadModuleRequest(int nModuleNo/*in*/, int nAddress/*in*/, int nCount/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/)
	{
		int nPduSize = 0;
		unsigned char *pHead = NULL;
		unsigned char szBuffer[512] = { 0 };    

		if ( (NULL == pBuffer)
			|| (nBufferSize < 0)
			|| (nAddress < 0)
			|| (nAddress > 0xFFFF)
			|| (nCount < 1)
			|| (nCount > 124)
			)
		{
			return -1;
		}
		
		pHead = szBuffer;
		*pHead++ = WECON_PLC_FUNCTION_CODE_READ;
		*pHead++ = BYTE(0x07);
		*pHead++ = FUNCTION_SUB_CODE_READ_MODULE;
		*pHead++ = BYTE(nModuleNo);
		*pHead++ = HIBYTE(nAddress);
		*pHead++ = LOBYTE(nAddress);
		*pHead++ = BYTE(nCount);
		nPduSize = (int)(pHead - szBuffer);
		if (nPduSize > nBufferSize)
		{
			return -2;
		}

		memcpy(pBuffer, szBuffer, nPduSize);

		return nPduSize;
	}

	/* data read module reply
	**/
	int CWeconPLCModbusPdu::dataReadModuleReply(unsigned char *pData/*in*/, int nDataSize/*in*/, int nModuleNo/*in*/, int nCount/*in*/,vector<int>& vecValue/*out*/)
	{
		unsigned char *pHead = NULL;
		int nPduSize = 0, nRecvSubCode = 0, nRecvModuleNo = 0, nRecvCount = 0;
		int i = 0, nValue = 0;
		if ((NULL == pData)
			|| (nDataSize < 4)
			)
		{
			return -1;
		}

		pHead = pData;
		nPduSize = *pHead++;
		nRecvSubCode = *pHead++;
		nRecvModuleNo = *pHead++;
		nRecvCount = *pHead++;
		if ((nPduSize != (5 + nCount * 2))
			|| (nRecvSubCode != FUNCTION_SUB_CODE_READ_MODULE)
			|| (nRecvModuleNo != nModuleNo)
			|| (nRecvCount != nCount)
			)
		{
			return -2;
		}

		vecValue.clear();
		vecValue.resize(nRecvCount);
		for (i = 0; i < nRecvCount; ++i)
		{
			vecValue[i] = getBigendianValueU16(pHead);
			pHead += 2;
		}

		return 0;
	}

	/* package write module request 
	**/
	int CWeconPLCModbusPdu::packageWriteModuleRequest(int nModuleNo/*in*/, int nAddress/*in*/, const vector<int>& vecValue/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/)
	{
		int nPduSize = 0;
		unsigned char *pHead = NULL;
		unsigned char szBuffer[1024] = { 0 };
		int nValueSize, i, nValue;

		if ((NULL == pBuffer)
			|| (nBufferSize < 7)
			|| (nModuleNo < 0)
			|| (nAddress < 0)
			|| (nAddress > 0xFFFF)
			|| ((nValueSize = vecValue.size()) < 0)
			|| (nValueSize > 123)
			)
		{
			return -1;
		}

		pHead = szBuffer;
		*pHead++ = WECON_PLC_FUNCTION_CODE_WRITE;
		*pHead++ = BYTE(7 + nValueSize * 2);
		*pHead++ = FUNCTION_SUB_CODE_WRITE_MODULE;
		*pHead++ = BYTE(nModuleNo);
		*pHead++ = HIBYTE(nAddress);
		*pHead++ = LOBYTE(nAddress);
		*pHead++ = BYTE(nValueSize);

		for (i = 0; i < nValueSize; ++i)
		{
			nValue = vecValue[i];
			*pHead++ = HIBYTE(nValue);
			*pHead++ = LOBYTE(nValue);
		}
		
		nPduSize = (int)(pHead - szBuffer);
		if (nPduSize > nBufferSize)
		{
			return -2;
		}
		memcpy(pBuffer, szBuffer, nPduSize);

		return nPduSize;
	}


	int CWeconPLCModbusPdu::packageReadInteriorRequest(int nAddress, int nCount, unsigned char* pBuffer, int nBufferSize)
	{
		int nPduSize = 0;
		unsigned char *pHead = NULL;
		unsigned char szBuffer[512] = { 0 };    

		if ( (NULL == pBuffer)
			|| (nBufferSize < 0)
			|| (nAddress < 0)
			|| (nAddress > 0xFFFF)
			|| (nCount < 1)
			|| (nCount > 124)
			)
		{
			return -1;
		}

		pHead = szBuffer;
		*pHead++ = WECON_PLC_FUNCTION_CODE_READ;
		*pHead++ = BYTE(0x06);
		*pHead++ = FUNCTION_SUB_CODE_RW_INTERIOR;
		*pHead++ = HIBYTE(nAddress);
		*pHead++ = LOBYTE(nAddress);
		*pHead++ = BYTE(nCount);
		nPduSize = (int)(pHead - szBuffer);

		if (nPduSize > nBufferSize)
		{
			return -2;
		}
		memcpy(pBuffer, szBuffer, nPduSize);

		return nPduSize;
	}

	/* data read interior reply 
	**/
	int CWeconPLCModbusPdu::dataWithReadInteriorReply(unsigned char *pData, int nDataSize, int nAddress, int nCount, vector<int> &vecValue)
	{
		unsigned char *pHead = NULL;
		int nPduSize = 0, nRecvSubCode = 0, nRecvAddress = 0, nRecvCount = 0;
		int i = 0;
		if ((NULL == pData)
			|| (nDataSize < 4)
			)
		{
			return -1;
		}

		pHead = pData;
		nPduSize = *pHead++;
		nRecvSubCode = *pHead++;
		nRecvAddress = getBigendianValueU16(pHead);
		pHead += 2;
		nRecvCount = *pHead++;

		if ((nPduSize != (6 + nCount * 2))
			|| (nRecvSubCode != FUNCTION_SUB_CODE_RW_INTERIOR)
			|| (nRecvAddress != nAddress)
			|| (nRecvCount != nCount)
			)
		{
			return -2;
		}

		vecValue.clear();
		vecValue.resize(nRecvCount);
		for (i = 0; i < nRecvCount; ++i)
		{
			vecValue[i] = getBigendianValueU16(pHead);
			pHead += 2;
		}

		return 0;
	}

	/* data write module reply 
	**/
	int CWeconPLCModbusPdu::dataWriteModuleReply(unsigned char *pData/*in*/, int nDataSize/*in*/, int nModuleNo/*in*/, int nAddress/*in*/, int nCount/*in*/)
	{
		unsigned char *pHead = NULL;
		int nReplyPduSize, nReplySubCode, nReplyModuleNo, nReplyAddress, nReplyCount;
		
		if ((NULL == pData)
			||(nDataSize < 6)
			)
		{
			return -1;
		}

		pHead = pData;
		nReplyPduSize = *pHead++;
		nReplySubCode = *pHead++;
		nReplyModuleNo = *pHead++;
		nReplyAddress = getBigendianValueU16(pHead);
		pHead += 2;
		nReplyCount = *pHead++;
		if ((nReplyPduSize != 7)
			||(nReplySubCode != FUNCTION_SUB_CODE_WRITE_MODULE)
			||(nReplyModuleNo != nModuleNo)
			|| (nReplyAddress != nAddress)
			||(nReplyCount != nCount)
			)
		{
			return -2;
		}

		return 0;
	}

	/* package read plc info request
	**/
	int CWeconPLCModbusPdu::packageReadPLCInfoRequest(int nSubCode, int nIndex, int nCount, unsigned char* pBuffer, int nBufferSize)
	{
		int nPduSize;
		unsigned char *pHead = NULL;
		unsigned char szBuffer[1024] = { 0 };

		if ((pBuffer == NULL)
			|| nBufferSize < 4)
		{
			return -1;
		}

		pHead = szBuffer;

		*pHead++ = BYTE(WECON_PLC_FUNCTION_CODE_READ);
		*pHead++ = BYTE(6);
		*pHead++ = BYTE(nSubCode);
		*pHead++ = BYTE(WECON_PLC_ENCRYPTION_VERSION);
		*pHead++ = BYTE(nIndex);
		*pHead++ = BYTE(nCount);
		nPduSize = pHead - szBuffer;
		if (nPduSize > nBufferSize)
		{
			return -2;
		}

		memcpy(pBuffer, szBuffer, nPduSize);

		return nPduSize;
		
	}

	/* unpackage read plc info request
	**/
	int CWeconPLCModbusPdu::dataWithReadPLCInfoReply(unsigned char *pData/*in*/, int nDataSize/*in*/, int nSubCode, int nCount, string &sResponse)
	{
		const unsigned char *pHead = NULL;
		int nPduSize, nRecvSubCode, nEncVersion, nResponse;
		
		if ( (pData == NULL)
			|| (nDataSize < 5))
		{
			return -1;
		}

		pHead = pData;
		nPduSize = *pHead++;
		nRecvSubCode = *pHead++;
		nEncVersion = *pHead++;
		nResponse = nPduSize - 4; 
		if ((nRecvSubCode != nSubCode)
			|| (nEncVersion != WECON_PLC_ENCRYPTION_VERSION)
			|| (nResponse != 2 * nCount)
			)
		{
			return -1;
		}

		if (!sResponse.empty())
		{
			sResponse.clear();
		}

		sResponse.assign((const char*)pHead, nResponse);

		return 0;
	}

	int CWeconPLCModbusPdu::packageRemoveDecTemporaryRequest(const vector<char>& vecKey, unsigned char* pBuffer, int nBuffersize)
	{
		unsigned char *pHead = NULL;
		unsigned char szBuffer[1024] = { 0 };
		int nPduSize, nSize, i;

		if ( (pBuffer == NULL)
			|| ((nSize = vecKey.size()) < 1)
			|| (nBuffersize < 5 + nSize)
			)
		{
			return -1;
		}

		pHead = szBuffer;
		*pHead++ = BYTE(WECON_PLC_FUNCTION_CODE_WRITE);
		*pHead++ = BYTE(5 + nSize);
		*pHead++ = BYTE(FUNCTION_SUB_CODE_REMOVE_TEMPORARY_DECRYPT);
		*pHead++ = BYTE(WECON_PLC_ENCRYPTION_VERSION);
		*pHead++ = BYTE(nSize);
		for (i = 0; i < nSize; ++i)
		{
			*pHead++ = vecKey[i];
		}
		
		nPduSize = pHead - szBuffer;
		if (nPduSize > nBuffersize)
		{
			return -2;
		}

		memcpy(pBuffer, szBuffer, nPduSize);

		return nPduSize;
	}

	int CWeconPLCModbusPdu::dataWithRemoveDecTemporaryReply(unsigned char *pData/*in*/, int nDataSize/*in*/, const vector<char>& vecKey)
	{
		const unsigned char *pHead = NULL;
		int nRecvPdu, nRecvSubCode, nRecvEncVersion, nErrorNo;

		if ( (pData == NULL)
			|| (nDataSize < 5)
			|| (vecKey.size() < 1)
			)
		{
			return -1;
		}

		pHead = pData;
		nRecvPdu = *pHead++;
		nRecvSubCode = *pHead++;
		nRecvEncVersion = *pHead++;
		nErrorNo = *pHead++;
		if ((nRecvPdu !=0x06)
			|| (nRecvSubCode != FUNCTION_SUB_CODE_REMOVE_TEMPORARY_DECRYPT)
			|| (nRecvEncVersion != WECON_PLC_ENCRYPTION_VERSION)
			|| (nErrorNo == 0)
			)
		{
			return -2;
		}

		return 0;
	}
	/* package operate passowrd code request
	**/
	int CWeconPLCModbusPdu::packageOperatePasswordRequest(int nSubCode, unsigned char cKeyType, const string& sPassword, const string& sKeySeed, unsigned char* pBuffer, int nBufferSize)
	{
		int nPduSize, nPwdLen, nRandom, i;
		unsigned char *pHead = NULL;
		unsigned char szBuffer[1024] = { 0 };
		short int nCrc16;
		CXorEncAndDec m_xor;

		if ((pBuffer == NULL) 
			|| (nBufferSize < 0)
			|| ((nPwdLen = sPassword.size()) < 8)
			|| nPwdLen > 16){
			return -1;
		}
		
		nRandom = rand();
		pHead = szBuffer;
		nPduSize = 4 + (5 + sizeof(nCrc16) + nPwdLen + 15) / 16 * 16;
		*pHead++ = BYTE(WECON_PLC_FUNCTION_CODE_WRITE);
		*pHead++ = BYTE(nPduSize);
		*pHead++ = BYTE(nSubCode);
		*pHead++ = BYTE(WECON_PLC_ENCRYPTION_VERSION);
		*pHead++ = nRandom & 0xFF;
		*pHead++ = BYTE(cKeyType);
		*pHead++ = (nRandom >> 8) & 0xFF;
		*pHead++ = BYTE(nPwdLen);
		*pHead++ = (nRandom >> 16) & 0xFF;

		if (nPduSize > nBufferSize){
			return -2;
		}

		for (i = 0; i < nPwdLen; i++){
			*pHead++ =  sPassword[i];
		}

		nCrc16 = cal_crc16(szBuffer + 4, nPduSize - sizeof(nCrc16) - 4);	
		pHead += nPduSize - 9 - nPwdLen - sizeof(nCrc16);
		*pHead++ = LOBYTE(nCrc16);
		*pHead++ = HIBYTE(nCrc16);
		memcpy(pBuffer, szBuffer, nPduSize);	
		if (m_xor.EncData(szBuffer + 4, pBuffer + 4, sKeySeed, nPduSize - 4) < 0){
			return -3;
		}

		return nPduSize;

	}

	

	/* package operate passowrd code reply
	**/
	int CWeconPLCModbusPdu::dataWithOperatePasswordReply(unsigned char* pData, int nDataSize, int nSubCode, int &nCode)
	{	
		unsigned char *pHead = NULL;
		int nPduSize, nRecvSubCode, nRecvKeyType;

		if ((pData == NULL)
			|| (nDataSize < 4))
		{
			return -1;
		}

		pHead = pData;
		nPduSize = *pHead++;
		nRecvSubCode = *pHead++;
		nRecvKeyType = *pHead++;
		nCode = *pHead++;
		if ((nPduSize != 5)
			|| (nRecvSubCode != nSubCode)
			|| (nRecvKeyType != WECON_PLC_ENCRYPTION_VERSION))
		{
			return -2;
		}

		if (nCode !=  0x01)
		{
			return -3;
		}

		return 0;
	}

	/* package write interior code request
	**/
	int CWeconPLCModbusPdu::packageWriteInteriorRequest(int nAddress/*in*/, const vector<int>& vecValue/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/)
	{
		int nPduSize = 0;
		unsigned char *pHead = NULL;
		unsigned char szBuffer[1024] = { 0 };
		int nValueSize, i, nValue;
		if ((NULL == pBuffer)
			|| (nBufferSize < 7)
			|| (nAddress < 0)
			|| (nAddress > 0xFFFF)
			|| ((nValueSize = vecValue.size()) < 0)
			|| (nValueSize > 123)
			)
		{
			return -1;
		}

		pHead = szBuffer;
		*pHead++ = WECON_PLC_FUNCTION_CODE_WRITE;
		*pHead++ = BYTE(6 + nValueSize * 2);
		*pHead++ = FUNCTION_SUB_CODE_RW_INTERIOR;
		*pHead++ = HIBYTE(nAddress);
		*pHead++ = LOBYTE(nAddress);
		*pHead++ = BYTE(nValueSize);

		for (i = 0; i < nValueSize; ++i)
		{
			nValue = vecValue[i];
			*pHead++ = HIBYTE(nValue);
			*pHead++ = LOBYTE(nValue);
		}

		nPduSize = (int)(pHead - szBuffer);
		if (nPduSize > nBufferSize)
		{
			return -2;
		}

		memcpy(pBuffer, szBuffer, nPduSize);

		return nPduSize;
	
	}

	int CWeconPLCModbusPdu::dataWithWriteInteriorReply(unsigned char *pData/*in*/, int nDataSize/*in*/, int nAddress/*in*/, int nCount/*in*/)
	{
		unsigned char *pHead = NULL;
		int nReplyPduSize, nReplySubCode, nReplyAddress, nReplyCount, nReplySuccess;

		if ((NULL == pData)
			||(nDataSize < 6)
			)
		{
			return -1;
		}

		pHead = pData;
		nReplyPduSize = *pHead++;
		nReplySubCode = *pHead++;
		nReplyAddress = getBigendianValueU16(pHead);
		pHead += 2;
		nReplyCount = *pHead++;
		nReplySuccess = *pHead++;
		if ((nReplyPduSize != 0x07)
			||(nReplySubCode != FUNCTION_SUB_CODE_RW_INTERIOR)
			|| (nReplyAddress != nAddress)
			||(nReplyCount != nCount)
			|| (nReplySuccess == 0x00)
			)
		{
			return -2;
		}

		return 0;
	}

	/* package write sub code request
	**/
	int CWeconPLCModbusPdu::packageWriteSubCodeRequest(int nSubCode/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/)
	{
		int nPduSize = 0;
		unsigned char *pHead = NULL;
		unsigned char szBuffer[1024] = { 0 };   

		if ( (NULL == pBuffer)
			||(nBufferSize < 0)
			){
				return -1;
		}

		pHead = szBuffer;
		*pHead++ = WECON_PLC_FUNCTION_CODE_WRITE_SUB_CODE;
		*pHead++ = BYTE(0x03);
		*pHead++ = BYTE(nSubCode);
		nPduSize = (int)(pHead -szBuffer);
		if (nPduSize > nBufferSize)
		{
			return -2;
		}

		memcpy(pBuffer, szBuffer, nPduSize);

		return nPduSize;
	}

	/* package write sub code reply
	**/
	int CWeconPLCModbusPdu::dataWriteSubCodeReply(unsigned char *pData/*in*/, int nDataSize/*in*/, int& nSubCode/*out*/)
	{
		int nValue = 0;
		unsigned char *pHead = NULL;
		int nPduSize = 0;

		if ((pData ==  NULL) || 
			nDataSize < 0)
		{
			return -1;
		}

		pHead = pData;
		nPduSize = *pHead++;
		if ((nPduSize - 1) != nDataSize)
		{
			return -2;
		}

		nSubCode = *pHead++;

		return 0;
	}	

	int CWeconPLCModbusPdu::dataWriteFileRecordReplyEx(unsigned char *pData/*in*/, int nDataSize/*in*/, int nFileNo/*in*/, int nRecordNo/*in*/, int nReferType/*in*/)
	{
		int nReplyFileNo = 0, nReplyRecordNo = 0;
		int i = 0, nDataBytes = 0, nReplynReferType = 0,  nReplyRecordSize = 0;
		unsigned char *pHead = NULL;

		/* check params */
		if((NULL == pData) || (nDataSize < 8)){
			return -1;
		}

		pHead = pData;
		nDataBytes = *pHead++;
		nReplynReferType = *pHead++;
		nReplyFileNo = getBigendianValueU16(pHead);
		pHead += 2;
		nReplyRecordNo= getBigendianValueU16(pHead);
		pHead += 2;
		nReplyRecordSize = getBigendianValueU16(pHead);
		pHead += 2;
		if ((nReferType != nReplynReferType)
			||(nReplyFileNo != nFileNo) 
			|| (nReplyRecordNo != nRecordNo)
			/*||(nDataBytes != 0x07)*/
			){
				return -3;
		}

		return 0;
	}
	/* check wecon user function code
	**/
	int CWeconPLCModbusPdu::checkUserFunctionCode(MODBUS_REPLY_HEADER_T& header, unsigned char *pData) const
	{  
		switch(header.nFuncCode)
		{
			case WECON_PLC_FUNCTION_CODE_WRITE_SUB_CODE:
			case WECON_PLC_FUNCTION_CODE_READ:
			case WECON_PLC_FUNCTION_CODE_WRITE:{
				header.nPduSize = *pData;
			}
			break;
		default:
			return -1;
		}
		return 0;
	}
	int CWeconPLCModbusPdu::cal_crc16(unsigned char *pData, int nSize) const{

		int i,j,nCrc16 = 0xFFFF;

		/* check params */
		if ((NULL == pData) || (nSize <= 0)){
			return -1;
		}

		/* calc crc16 */
		for (i = 0; i < nSize; i++){
			nCrc16 ^= pData[i];
			for (j = 0; j < 8; j++){
				if (nCrc16 & 0x01){
					nCrc16 = (nCrc16 >> 1) ^ 0xa001;
				}
				else{
					nCrc16 >>= 1;
				}
			}
		}

		return nCrc16;
	}
}


