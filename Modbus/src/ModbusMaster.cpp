#include "ModbusMaster.h"
namespace cbl{
	/* construct
	**/
	CModbusMaster::CModbusMaster(){
		m_nSalveId = 1;
		m_nSessionId = 1;
		m_pModbusPdu = &m_modbusPdu;
		setTransferMode(MODBUS_TRANSFER_RTU);
	}
	/* destruct
	**/
	CModbusMaster::~CModbusMaster(){
		
	}

	/* set slave id
	**/
	int CModbusMaster::setSlaveId(int nId){

		CAutoLock locker(&m_mutex);

		m_nSalveId = nId;

		return 0;
	}

	/* get slave id
	**/
	int CModbusMaster::getSlaveId(){

		CAutoLock locker(&m_mutex);

		return m_nSalveId;
	}

	/* set transfer mode
	**/
	int CModbusMaster::setTransferMode(MODBUS_TRANSFER_T mode){

		CAutoLock locker(&m_mutex);

		m_transferMode = mode;
		m_nOffset = (MODBUS_TRANSFER_RTU == m_transferMode) ? 2 : 8;

		return 0;
	}

	/* get transfer mode
	**/
	MODBUS_TRANSFER_T CModbusMaster::getTransferMode(){

		CAutoLock locker(&m_mutex);

		return m_transferMode;
	}
	
	/* package read coils
	**/
	int CModbusMaster::packageReadCoils(int nAddress, int nCount, unsigned char *pBuffer, int nBufferSize){
		int nPduSize;
		unsigned char szBuffer[512] = { 0 };
		CAutoLock locker(&m_mutex);

		/* make pdu data */
		if ((nPduSize = m_pModbusPdu->packageReadCoilsRequest(nAddress, nCount, szBuffer, sizeof(szBuffer))) <= 0){
			return (nPduSize - 32);
		}

		return this->packageRequest(szBuffer, nPduSize, pBuffer, nBufferSize);
	}

	/* package read registers
	**/
	int CModbusMaster::packageReadRegisters(int nAddress, int nCount, unsigned char *pBuffer, int nBufferSize){

		int nPduSize;
		unsigned char szBuffer[512] = { 0 };
		CAutoLock locker(&m_mutex);

		/* make pdu data */
		if ((nPduSize = m_pModbusPdu->packageReadRegistersRequest(nAddress, nCount, szBuffer, sizeof(szBuffer))) <= 0){
			return (nPduSize - 32);
		}

		return this->packageRequest(szBuffer, nPduSize, pBuffer, nBufferSize);
	}
	/*package read file record
	**/
	int CModbusMaster::packageReadFileRecord(int nFileNo/*in*/, int nRecordNo/*in*/, int nRecordSize/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*out*/)
	{
		int nPduSize;
		unsigned char szBuffer[512] = { 0 };
		CAutoLock locker(&m_mutex);
		/* make pdu data */
		if ((nPduSize = m_pModbusPdu->packageReadFileRecordRequest(nFileNo, nRecordNo, nRecordSize, szBuffer, sizeof(szBuffer))) <= 0){
			return (nPduSize - 32);
		}

		return this->packageRequest(szBuffer, nPduSize, pBuffer, nBufferSize);
	}
	/* package write single coil
	**/
	int CModbusMaster::packageWriteSingleCoil(int , bool , unsigned char *, int){

		return -127;
	}


	/* packete write single register
	**/
	int CModbusMaster::packageWriteSingleRegister(int nAddress, int nValue, unsigned char *pBuffer, int nBufferSize){

		int nPduSize;
		unsigned char szBuffer[512] = { 0 };
		CAutoLock locker(&m_mutex);

		/* make pdu data */
		if ((nPduSize = m_pModbusPdu->packageWriteSingleRegisterRequest(nAddress, nValue, szBuffer, sizeof(szBuffer))) <= 0){
			return (nPduSize - 32);
		}

		return this->packageRequest(szBuffer, nPduSize, pBuffer, nBufferSize);
	}

	/* package write coils
	**/
	int CModbusMaster::packageWriteCoils(int nAddress, const vector<int> &vecValue, unsigned char *pBuffer, int nBufferSize){

		int nPduSize;
		unsigned char szBuffer[512] = { 0 };
		CAutoLock locker(&m_mutex);

		/* make pdu data */
		if ((nPduSize = m_pModbusPdu->packageWriteCoilsRequest(nAddress, vecValue, szBuffer, sizeof(szBuffer))) <= 0){
			return (nPduSize - 32);
		}

		return this->packageRequest(szBuffer, nPduSize, pBuffer, nBufferSize);
	}

	/* package write registers
	**/
	int CModbusMaster::packageWriteRegisters(int nAddress, const vector<int>& vecValue, unsigned char *pBuffer, int nBufferSize){

		int nPduSize;	
		unsigned char szBuffer[512] = { 0 };
		CAutoLock locker(&m_mutex);

		/* make pdu data */
		if ((nPduSize = m_pModbusPdu->packageWriteRegistersRequest(nAddress, vecValue, szBuffer, sizeof(szBuffer))) <= 0){
			return (nPduSize - 32);
		}

		return this->packageRequest(szBuffer, nPduSize, pBuffer, nBufferSize);
	}
	int CModbusMaster::packageWriteFileRecord(int nFileNo/*in*/, int nRecordNo/*in*/, int nReferType/*in*/, const unsigned char *pData/*in*/, int nDataSize/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/)
	{	
		int nPduSize;	
		unsigned char szBuffer[512] = { 0 };
		CAutoLock locker(&m_mutex);

		/* make pdu data */
		if ((nPduSize = m_pModbusPdu->packageWriteFileRecordRequest(nFileNo, nRecordNo, nReferType, pData, nDataSize, szBuffer, sizeof(szBuffer))) <= 0){
			return (nPduSize - 32);
		}

		return this->packageRequest(szBuffer, nPduSize, pBuffer, nBufferSize);
	}
	/* do package
	**/
	int CModbusMaster::packageRequest(unsigned char *pPduData, int nPduSize, unsigned char *pBuffer, int nBufferSize){

		int nCrc16, nPackageSize;
		unsigned char szBuffer[512] = { 0 };
		unsigned char *pHead = szBuffer;

		/* check params */
		if ((NULL == pPduData) || (nPduSize <= 0) || (NULL == pBuffer) || (nBufferSize <= 0)){
			return -1;
		}

		/* fill tcp header */
		if (MODBUS_TRANSFER_TCP == m_transferMode){

			int nLength = nPduSize + 1;

			/* fill header */
			*pHead++ = BYTE(m_nSessionId >> 8);
			*pHead++ = BYTE(m_nSessionId);
			*pHead++ = 0;
			*pHead++ = 0;
			*pHead++ = BYTE(nLength >> 8);
			*pHead++ = BYTE(nLength);

			/* increase session id */
			m_nSessionId++;
			if (m_nSessionId > 0x7FFF){
				m_nSessionId = 1;
			}
		}

		/* fill slave id */
		*pHead++ = BYTE(m_nSalveId);

		/* fill pdu data */
		memcpy(pHead, pPduData, nPduSize);
		pHead += nPduSize;

		/* fill rtu crc16 */
		if (MODBUS_TRANSFER_RTU == m_transferMode){
			nCrc16 = cal_crc16(szBuffer, (int)(pHead - szBuffer));
			*pHead++ = BYTE(nCrc16);
			*pHead++ = BYTE(nCrc16 >> 8);		
		}

		/* copy buffer */
		nPackageSize = (int)(pHead - szBuffer);
		if (nPackageSize > nBufferSize){
			return -2;
		}
		memcpy(pBuffer, szBuffer, nPackageSize);

		return nPackageSize;
	}

	/* unpackage read coils
	**/
	int CModbusMaster::unpackageReadCoils(unsigned char *pData, int nDataSize, int nCoilsCount, vector<int>& vecValue){

		CAutoLock locker(&m_mutex);
		int nPduSize, nFuncCode;

		/* check reply package */
		if (checkReplyPackage(pData, nDataSize, nFuncCode, nPduSize) < 0){
			return -1;
		}

		/* check func code */
		if (FUNCTION_CODE_READ_COILS != nFuncCode){
			return -2;
		}

		/* get payload data */
		if (m_pModbusPdu->dataReadCoilsReply(pData + m_nOffset, nPduSize - 1, nCoilsCount, vecValue) < 0){
			return -3;
		}

		return 0;
	}
	/* unpackage read registers
	*/
	int CModbusMaster::unpackageReadRegisters(unsigned char *pData, int nDataSize, vector<int>& vecValue){

		CAutoLock locker(&m_mutex);
		int nPduSize, nFuncCode;

		/* check reply package */
		if (checkReplyPackage(pData, nDataSize, nFuncCode, nPduSize) < 0){
			return -1;
		}

		/* check func code */
		if (FUNCTION_CODE_READ_REGISTERS != nFuncCode){
			return -2;
		}

		/* get payload data */
		if (m_pModbusPdu->dataReadRegistersReply(pData + m_nOffset, nPduSize - 1, vecValue) < 0){
			return -3;
		} 

		return 0;
	}

	/* unpackage read file
	**/
	int CModbusMaster::unpakageReadFileRecord(unsigned char *pData/*in*/, int nDataSize/*in*/,  unsigned char *pBuffer/*out*/, int nMaxBufferSize/*in*/, int& nBufferSize/*out*/)
	{
		CAutoLock locker(&m_mutex);
		int nPduSize, nFuncCode, ret;

		/* check reply package */
		ret = checkReplyPackage(pData, nDataSize, nFuncCode, nPduSize);
		if (ret < 0){
			return ret;
		}

		if (FUNCTION_CODE_READ_FILERECORD != nFuncCode)
		{
			return -2;
		}

		/* get payload data */
		if (m_pModbusPdu->dataReadFileRecordReply(pData + m_nOffset, nPduSize - 1, pBuffer, nMaxBufferSize, nBufferSize) < 0){
			return -3;
		}

		return 0;
	}
	/* unpackage write single coil
	**/
	int CModbusMaster::unpackageWriteSingleCoil(unsigned char *pData/*in*/, int nDataSize/*in*/, int& nAddress/*out*/, bool &bOn/*out*/)
	{
		return 0;
	}
	/* unpackage write single coil
	**/
	int unpackageWriteSingleRegister(unsigned char *pData/*in*/, int nDataSize/*in*/, int &nAddress/*out*/, int &nValue/*out*/)
	{
		return 0;
	}
	/* unpackage write coils
	**/
	int CModbusMaster::unpackageWriteCoils(unsigned char *pData, int nDataSize, int& nStartAdress, int& nCoilsCount){

		CAutoLock locker(&m_mutex);
		int nPduSize, nFuncCode;

		/* check reply package */
		if (checkReplyPackage(pData, nDataSize, nFuncCode, nPduSize) < 0){
			return -1;
		}

		/* check func code */
		if (FUNCTION_CODE_WRITE_COILS != nFuncCode){
			return -2;
		}

		/* get payload data */
		if (m_pModbusPdu->dataWriteCoilsReply(pData + m_nOffset, nPduSize - 1, nStartAdress, nCoilsCount) < 0){
			return -3;
		}

		return 0;
	}


	/* unpackage write registers
	**/
	int CModbusMaster::unpackageWriteRegisters(unsigned char *pData, int nDataSize, int& nStartAdress, int& nRegisterCount){

		CAutoLock locker(&m_mutex);
		int nPduSize, nFuncCode;

		/* check reply package */
		if (checkReplyPackage(pData, nDataSize, nFuncCode, nPduSize) < 0){
			return -1;
		}

		/* check func code */
		if (FUNCTION_CODE_WRITE_REGISTERS != nFuncCode){

			return -2;//-(*(pData + 2));
		}

		/* get payload data */
		if (m_pModbusPdu->dataWriteRegistersReply(pData + m_nOffset, nPduSize - 1, nStartAdress, nRegisterCount) < 0){
			return -3;
		}

		return 0;
	}

	/* unpackage write file
	**/
	int CModbusMaster::unpakageWriteFileRecord(unsigned char *pData/*in*/, int nDataSize/*in*/, int nFileNo/*in*/, int nRecordNo/*in*/, int nReferType/*in*/, unsigned char *pBuffer/*out*/, int nMaxBufferSize/*in*/, int& nBufferSize/*out*/)
	{
		CAutoLock locker(&m_mutex);
		int nPduSize, nFuncCode;

		/* check reply package */
		if (checkReplyPackage(pData, nDataSize, nFuncCode, nPduSize) < 0){
			return -1;
		}

		if (FUNCTION_CODE_WRITE_FILERECORD != nFuncCode)
		{
			return -2;
		}

		/* get payload data */
		if (m_pModbusPdu->dataWriteFileRecordReply(pData + m_nOffset, nPduSize - 1, nFileNo, nRecordNo, nReferType, pBuffer, nMaxBufferSize, nBufferSize) < 0){
			return -3;
		}
		
		return 0;
	}



	/* check reply package
	**/
	int CModbusMaster::checkReplyPackage(unsigned char *pData, int nDataSize,int& nFuncCode,int& nPduSize){

		int nRtn, nPayloadSize, nCrc16;
		unsigned char *pHead = pData;
		MODBUS_REPLY_HEADER_T header;

		/* check params */
		if ((NULL == pData) || (nDataSize < 4)){
			return -1;
		}

		/* check tcp header */
		if (MODBUS_TRANSFER_TCP == m_transferMode){

			if (nDataSize < 8){
				return -2;
			}

			if (BYTE(pData[2]) || BYTE(pData[3])){
				return -3;
			}

			nPayloadSize = (BYTE(pData[4]) << 8) | BYTE(pData[5]);
			if (nDataSize < nPayloadSize + 6){
				return -4;
			}

			pHead = pData + 6;
		}

		/* check slave id */
		if (m_nSalveId != BYTE(*pHead)){
			return -4;
		}
		pHead++;

		/* check pdu header */
		if ((nRtn = m_pModbusPdu->checkPduHeader(pHead, nDataSize - int(pHead - pData), header)) < 0){
			return -5;
		}

		/* check crc32 value */
		if (MODBUS_TRANSFER_RTU == m_transferMode){
			pHead = pData + header.nPduSize + 1;
			//nCrc16 = ((*(pHead + 1)) << 8) | (*pHead);
			nCrc16 = *((unsigned short *)pHead);
			if (cal_crc16(pData, header.nPduSize + 1) != nCrc16){
				return -6;
			}
		}
		
		if (header.nFuncCode & 0x80) {
			return -(MODBUS_ERROR_OFFSET + header.nErrorCode);
		}
		/* assign pdu size */
		nFuncCode = header.nFuncCode;
		nPduSize = header.nPduSize;

		return 0;
	}

	/* find reply package
	**/
	int CModbusMaster::findReplyPackage(unsigned char *pData, int nDataSize, int& nPackageSize){

		CAutoLock locker(&m_mutex);
		unsigned char *pHead = NULL;
		int i, nFuncCode, nPduSize, nCheckSize = nDataSize - ((MODBUS_TRANSFER_RTU == m_transferMode) ? 4 : 8) + 1;

		if ((NULL == pData) || (nCheckSize <= 0)){
			return -1;
		}
		//stp_mem(pData,nDataSize);
		for (i = 0, pHead = pData; i < nCheckSize; i++, pHead++){

			if (this->checkReplyPackage(pHead, nDataSize - i, nFuncCode, nPduSize) < 0){
				continue;
			}
			nPackageSize = nPduSize + ((MODBUS_TRANSFER_TCP == m_transferMode) ? 7 : 3);

			return i;
		}

		return -2;
	}

	MODBUS_TRANSFER_T CModbusMaster::getMode()
	{
		return m_transferMode;
	}

	int CModbusMaster::cal_crc16(unsigned char *pData, int nSize){

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