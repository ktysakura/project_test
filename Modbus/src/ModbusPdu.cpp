#include "ModbusPdu.h"

namespace cbl{

	/* construct
	**/
	CModbusPdu::CModbusPdu(){
	
	}

	/* destruct
	**/
	CModbusPdu::~CModbusPdu(){
	
	}

	/* package read coils request
	**/
	int CModbusPdu::packageReadCoilsRequest(int nAddress, int nCount, unsigned char *pBuffer, int nBufferSize) const{

		int nPduSize;
		unsigned char *pHead = NULL;
		unsigned char szBuffer[4096] = { 0 };

		/* check params */
		if ((nAddress < 0)
			|| (nAddress >= 0x10000)
			|| (nCount < 1)
			|| (nCount > READ_COILS_MAX_COUNT) 
			|| (NULL == pBuffer)
			|| (nBufferSize <= 0)){
			return -1;
		}

		/* package data */
		pHead = szBuffer;
		*pHead++ = BYTE(FUNCTION_CODE_READ_COILS);
		*pHead++ = BYTE(nAddress >> 8);
		*pHead++ = BYTE(nAddress);
		*pHead++ = BYTE(nCount >> 8);
		*pHead++ = BYTE(nCount);

		/* copy buffer */
		nPduSize = (int)(pHead - szBuffer);
		if (nPduSize > nBufferSize){
			return -2;
		}
		memcpy(pBuffer, szBuffer, nPduSize);

		return nPduSize;
	}

	/* package read registers request
	*/
	int CModbusPdu::packageReadRegistersRequest(int nAddress, int nCount, unsigned char *pBuffer, int nBufferSize) const{

		int nPduSize;
		unsigned char *pHead = NULL;
		unsigned char szBuffer[4096] = { 0 };

		/* check params */
		if ((nAddress < 0)
			|| (nAddress >= 0x10000)
			|| (nCount < 1)
			|| (nCount > READ_REGISTERS_MAX_COUNT) //125
			|| (NULL == pBuffer)
			|| (nBufferSize <= 0)){
			return -1;
		}

		/* package data */
		pHead = szBuffer;
		*pHead++ = BYTE(FUNCTION_CODE_READ_REGISTERS);
		*pHead++ = BYTE(nAddress >> 8);
		*pHead++ = BYTE(nAddress);
		*pHead++ = BYTE(nCount >> 8);
		*pHead++ = BYTE(nCount);

		/* copy buffer */
		nPduSize = (int)(pHead - szBuffer);
		if (nPduSize > nBufferSize){
			return -2;
		}
		memcpy(pBuffer, szBuffer, nPduSize);

		return nPduSize;
	}

	/* package write single coil request
	**/
	int CModbusPdu::packageWriteSingleCoilRequest(int nAddress, int nValue, unsigned char *pBuffer, int nBufferSize) const{

		int nPduSize;
		unsigned char *pHead = NULL;
		unsigned char szBuffer[512] = { 0 };    

		/* check params */
		if ((nAddress < 0)
			|| (nAddress >= 0x10000)
			|| (NULL == pBuffer)
			|| (nBufferSize <= 0)){
			return -1;
		}
		nValue = ((nValue > 0) ? 0xFF00 : 0x0000);

		/* add header */
		pHead = szBuffer;
		*pHead++ = BYTE(FUNCTION_CODE_WRITE_SINGLE_COIL);
		*pHead++ = BYTE(nAddress >> 8);
		*pHead++ = BYTE(nAddress);
		*pHead++ = BYTE(nValue >> 8);
		*pHead++ = BYTE(nValue);

		/* copy buffer */
		nPduSize = (int)(pHead - szBuffer);
		if (nPduSize > nBufferSize){
			return -2;
		}
		memcpy(pBuffer, szBuffer, nPduSize);

		return nPduSize;
	}

	/* packete write single register request
	**/
	int CModbusPdu::packageWriteSingleRegisterRequest(int nAddress, int nValue, unsigned char *pBuffer, int nBufferSize) const{

		int nPduSize;
		unsigned char *pHead = NULL;
		unsigned char szBuffer[4096] = { 0 };

		/* check params */
		if ((nAddress < 0)
			|| (nAddress >= 0x10000)
			|| (NULL == pBuffer)
			|| (nBufferSize <= 0)){
			return -1;
		}

		/* add header */
		pHead = szBuffer;
		*pHead++ = BYTE(FUNCTION_CODE_WRITE_SINGLE_REGISTER);
		*pHead++ = BYTE(nAddress >> 8);
		*pHead++ = BYTE(nAddress);
		*pHead++ = BYTE(nValue >> 8);
		*pHead++ = BYTE(nValue);

		/* copy buffer */
		nPduSize = (int)(pHead - szBuffer);
		if (nPduSize > nBufferSize){
			return -2;
		}
		memcpy(pBuffer, szBuffer, nPduSize);

		return nPduSize;
	}

	/* package write coils request
	**/
	int CModbusPdu::packageWriteCoilsRequest(int nAddress, const vector<int> &vecValue, unsigned char *pBuffer, int nBufferSize) const{

		int i, j, nPduSize, nByteSize, nValue = 0, nValueSize = -1;
		unsigned char *pHead = NULL;
		unsigned char szBuffer[4096] = { 0 };

		/* check params */
		if ((nAddress < 0)
			|| (nAddress >= 0x10000)
			|| ((nValueSize = (int)vecValue.size()) <= 0)
			|| (nValueSize > WRITE_COILS_MAX_COUNT)/*0x07B0 = 1968*/
			|| (NULL == pBuffer)
			|| (nBufferSize <= 0)
			){
				printf("nAddress = %d,nAddress= %08x,nValueSize = %d,pBuffer = %p, nBufferSize = %d\n",
					nAddress, nAddress, nValueSize, pBuffer, nBufferSize);
			return -1;
		}

		/* add header */
		pHead = szBuffer;
		*pHead++ = BYTE(FUNCTION_CODE_WRITE_COILS);
		*pHead++ = BYTE(nAddress >> 8);
		*pHead++ = BYTE(nAddress);
		*pHead++ = BYTE(nValueSize >> 8);
		*pHead++ = BYTE(nValueSize);

		nByteSize = (nValueSize + 7) / 8;
		*pHead++ = BYTE(nByteSize);

		/* add data */
		for (i = 0; i < nByteSize; i++){		
			for (j = 0; j < 8; j++){
				if ( 8 * i + j >= nValueSize){
					break;
				}
				/*set on*/
				if(vecValue[8 * i + j])
				{
					nValue |= 1 << j;
				}
				/*set off*/
				else
				{		
					nValue &= ~(1 << j);
				}
			}
			*pHead++ = BYTE(nValue);
			nValue >>= 8;
		}

		/* copy buffer */
		nPduSize = (int)(pHead - szBuffer);
		if (nPduSize > nBufferSize){
			return -2;
		}
		memcpy(pBuffer, szBuffer, nPduSize);

		return nPduSize;
	}

	/* package write registers request
	**/
	int CModbusPdu::packageWriteRegistersRequest(int nAddress, const vector<int>& vecValue, unsigned char *pBuffer, int nBufferSize) const{

		int i, nValue, nPduSize, nValueSize = -1;
		unsigned char *pHead = NULL;
		unsigned char szBuffer[4096] = { 0 };

		/* check params */
		if ((nAddress < 0)
			|| (nAddress >= 0x10000)
			|| ((nValueSize = (int)vecValue.size()) <= 0)
			|| (nValueSize > WRITE_REGISTERS_MAX_COUNT)
			|| (NULL == pBuffer)
			|| (nBufferSize <= 0)
			){
				printf("nAddress = %d,nAddress= %08x,nValueSize = %d,pBuffer = %p, nBufferSize = %d\n",
					nAddress, nAddress, nValueSize, pBuffer, nBufferSize);
			return -1;
		}

		/* add header */
		pHead = szBuffer;
		*pHead++ = BYTE(FUNCTION_CODE_WRITE_REGISTERS);
		*pHead++ = BYTE(nAddress >> 8);
		*pHead++ = BYTE(nAddress);
		*pHead++ = BYTE(nValueSize >> 8);
		*pHead++ = BYTE(nValueSize);
		*pHead++ = BYTE(nValueSize * 2);

		/* add data */
		for (i = 0; i < nValueSize; i++){
			nValue = vecValue[i];
			*pHead++ = BYTE(nValue >> 8);
			*pHead++ = BYTE(nValue);
		}

		/* copy buffer */
		nPduSize = (int)(pHead - szBuffer);
		if (nPduSize > nBufferSize){
			return -2;
		}
		memcpy(pBuffer, szBuffer, nPduSize);

		return nPduSize;
	}
	int CModbusPdu::packageWriteFileRecordRequest(int nFileNo/*in*/, int nRecordNo/*in*/, int nReferType/*in*/, const unsigned char *pData/*in*/, int nDataSize/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/)
	{
		int nPduSize, nValueSize, nRequestSize;
		unsigned char *pHead = NULL;
		unsigned char szBuffer[4096] = { 0 };

		;
		/* check params */
		if ((nFileNo < 0)
			|| (nFileNo > 0xFFFF)  /*65536*/
			|| (nRecordNo < 0)
			|| (nRecordNo > 0X270F)/*10000*/
			|| ((nValueSize = nDataSize) <= 0)
			|| ((nRequestSize = (7 + nValueSize)) < 0x07)
			|| (nRequestSize > 0xF5)
			|| (NULL == pBuffer) 
			|| (nBufferSize < 0)
			){
				printf(
					"CModbusPdu packageWriteFileRecordRequest: nFileNo = %d, nRecordNo= %d, nValueSize = %d, nRequestSize = %d, pBuffer = %p, nBufferSize = %d\n",
					nFileNo, nRecordNo, nValueSize, nRequestSize, pBuffer, nBufferSize);
				return -1;
		}

		pHead = szBuffer;
		*pHead++ = BYTE(FUNCTION_CODE_WRITE_FILERECORD);
		*pHead++ = BYTE(nRequestSize);
		*pHead++ = BYTE(nReferType);
		*pHead++ = HIBYTE(nFileNo);
		*pHead++ = LOBYTE(nFileNo);
		*pHead++ = HIBYTE(nRecordNo);
		*pHead++ = LOBYTE(nRecordNo);
		*pHead++ = HIBYTE(nValueSize / 2);
		*pHead++ = LOBYTE(nValueSize / 2);

		/* copy buffer */
		nPduSize = (int)(pHead - szBuffer) + nValueSize;
		if (nPduSize > nBufferSize)
		{
			return -4;
		}
		memcpy(pHead, pData, nValueSize);
		memcpy(pBuffer, szBuffer, nPduSize);

		return nPduSize;
	}

	int CModbusPdu::packageReadFileRecordRequest(int nFileNo/*in*/, int nRecordNo/*in*/, int nRecordSize/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*out*/)
	{
		int nPduSize, nRequestSize = 0;
		unsigned char *pHead = NULL;
		unsigned char szBuffer[4096] = { 0 };


		if (
			
			((nRequestSize = nRecordSize + 7) < 0x07)
			|| (nRequestSize > 0xF5) /*246*/
			|| (nFileNo < 0x0)
			|| (nFileNo > 0xFFFF)    /*65536*/ 
			|| (nRecordNo < 0x0)
			|| (nRecordNo > 0x270F)  /*10000*/
			){
				printf(
					"CModbusPdu packageReadFileRecordRequest: nFileNo = %d, nRecordNo = %d, nRequestSize = %d, nRecordSize = %d\n",
					nFileNo, nRecordNo, nRequestSize, nRecordSize);
				return -1;
			 }

		pHead = szBuffer;
		*pHead++ = BYTE(FUNCTION_CODE_READ_FILERECORD);
		*pHead++ = BYTE(0x06);
		*pHead++ = BYTE(0x06);
		*pHead++ = HIBYTE(nFileNo);
		*pHead++ = LOBYTE(nFileNo);
		*pHead++ = HIBYTE(nRecordNo);
		*pHead++ = LOBYTE(nRecordNo);
		*pHead++ = HIBYTE(nRecordSize/2);
		*pHead++ = LOBYTE(nRecordSize/2);
		nPduSize = (int)(pHead - szBuffer);
		/* copy buffer */
		if (nPduSize > nBufferSize)
		{
			return -2;
		}
		memcpy(pBuffer, szBuffer, nPduSize);

		return nPduSize;
	}
	/* data for read coils reply
	**/
	int CModbusPdu::dataReadCoilsReply(unsigned char *pData, int nDataSize, int nCoilsCount, vector<int>& vecValue) const{

		int nValue = 0;
		int i, j, nBytes, nIndex = 0;
		unsigned char *pHead = NULL;

		/* check params */
		if((NULL == pData) || (nDataSize < 1)){
			return -1;
		}

		pHead = pData;

		/* check bytes */
		nBytes = (*pHead++);
		if(((nCoilsCount + 7) / 8) != nBytes){
			return -2;
		}

		/* get Coils state */
		vecValue.clear();
		for (i = 0; i < nBytes; i++){
			nValue = *pHead++;
			for (j = 0; j < 8; j++){
				if (nIndex >= nCoilsCount){
					break;
				}
				vecValue.push_back(nValue & 0x01);
				nValue >>= 1;
				nIndex++;
			}
		}

		return 0;
	}

	/* data for read registers reply
	**/
	int CModbusPdu::dataReadRegistersReply(unsigned char *pData, int nDataSize, vector<int>& vecValue) const{

		short nValue;
		int i, nBytes, nSize;
		unsigned char *pHead = NULL;	

		/* check params */
		if((NULL == pData) || (nDataSize < 1)){
			return -1;
		}
		
		/* check bytes */
		nBytes = (*pData);
		if((0 != nBytes % 2) || (nDataSize < nBytes + 1)){
			return -2;
		}
		
		/* get registers value */
		vecValue.clear();
		pHead = pData + 1;
	//	stp_mem(pHead, nBytes);
		nSize = nBytes / 2;
		for (i = 0; i < nSize; i++){
			nValue = *pHead++;
			nValue <<= 8;
			nValue |= *pHead++;
			vecValue.push_back(nValue);
		}

		return 0;
	}

	/* data for write coils reply
	**/
	int CModbusPdu::dataWriteCoilsReply(unsigned char *pData, int nDataSize, int& nStartAdress, int& nCoilsCount) const{
		/* check params */
		if((NULL == pData) || (nDataSize < 4)){
			return -1;
		}

		/* parse payload */
		nStartAdress = getBigendianValueU16(pData);
		nCoilsCount = getBigendianValueU16(pData + 2);

		return 0;
	}

	/* data for write registers reply
	**/
	int CModbusPdu::dataWriteRegistersReply(unsigned char *pData, int nDataSize, int& nStartAdress, int& nRegisterCount) const{

		/* check params */
		if((NULL == pData) || (nDataSize < 4)){
			return -1;
		}

		/* parse payload */
		nStartAdress = getBigendianValueU16(pData);
		nRegisterCount = getBigendianValueU16(pData + 2);

		return 0;
	}

	/* data for write file record
	**/
	int CModbusPdu::dataWriteFileRecordReply(unsigned char *pData/*in*/, int nDataSize/*in*/, int nFileNo/*in*/, int nRecordNo/*in*/, int nReferType/*in*/, unsigned char *pBuffer/*out*/, int nBufferMaxSize /*in*/,int& nBufferSize/*out*/)
	{
		int nReplyFileNo = 0, nReplyRecordNo = 0;
		int i = 0, nDataBytes = 0, nReplynReferType = 0,  nReplyRecordSize = 0;
		unsigned char *pHead = NULL;
		
		/* check params */
		if((NULL == pData) || (nDataSize < 8)){
			return -1;
		}

		pHead = pData;
		nDataBytes = (*pHead++) - 7;
		if (nDataBytes % 2)
		{
			return -2;
		}

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
			||(nReplyRecordSize != nDataBytes / 2)
			||(nDataBytes > nBufferMaxSize)
			){
			return -3;
		}

		nBufferSize = nDataBytes;
		memcpy(pBuffer, pHead, nBufferSize);

		return 0;
	}

	/* data for read file record
	**/
	int CModbusPdu::dataReadFileRecordReply(unsigned char *pData/*in*/, int nDataSize/*in*/,  unsigned char *pBuffer/*out*/, int nMaxBufferSize/*in*/, int& nBufferSize/*out*/)
	{
		int i = 0, nRecordSize = 0, nRefNo = 0, nPduSize = 0, nReplySize = 0;
		unsigned char* pHead = NULL;
		
		if ((NULL == pData) || nDataSize < 3)
		{
			return -1;
		}

		pHead = pData;
		nPduSize = *pHead++;
		nRecordSize = BYTE(*pHead++);
		nRefNo = BYTE(*pHead++);
		nReplySize = nRecordSize - 1;
		if ((nReplySize > nMaxBufferSize)
			|| (nDataSize - 1 != nPduSize)
			|| (6 != nRefNo)
			)
		{
			return -2;
		}
		
		memcpy(pBuffer, pHead, nReplySize);
		nBufferSize = nReplySize;

		return 0;
	}
	/* check reply header
	**/
	int CModbusPdu::checkPduHeader(unsigned char *pData, int nDataSize, MODBUS_REPLY_HEADER_T& header) const{

		/* check params */
		if ((NULL == pData) || (nDataSize < 2)){
			return -1;
		}

		/* parse data */
		header.nFuncCode = BYTE(*pData);
		if (0 == (header.nFuncCode & 0x80)){
			header.nErrorCode = 0;
			
				switch (header.nFuncCode)
				{
				case FUNCTION_CODE_READ_COILS:
				case FUNCTION_CODE_READ_REGISTERS:
				case FUNCTION_CODE_WRITE_FILERECORD:
				case FUNCTION_CODE_READ_FILERECORD:{
					header.nPduSize = 2 + *(pData + 1);
					break;
				}	

				case FUNCTION_CODE_WRITE_SINGLE_COIL:{
					header.nPduSize = 4;
				}
				case FUNCTION_CODE_WRITE_COILS:
				case FUNCTION_CODE_WRITE_REGISTERS:{
					header.nPduSize = 5;
					break;
				}
				default:
					/*user self function code */
					if (checkUserFunctionCode(header, pData + 1) < 0)
					{
						return -2;
					}
					break;
			}
			
			if(nDataSize < header.nPduSize){
				return -3;
			}		
		}
		else{
			/*错误码*/
			header.nErrorCode = *(pData + 1);
			header.nPduSize = 2;
		}

		return 0;
	}
}