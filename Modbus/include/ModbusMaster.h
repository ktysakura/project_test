#if !defined(MODBUS_MASTER_H__20190408)
#define MODBUS_MASTER_H__20190408

#include <stdio.h>
#include <stdlib.h>
#include "ModbusPdu.h"
#include "MutexLock.h"
#include "AutoLock.h"

namespace cbl{

	typedef enum{
		MODBUS_TRANSFER_RTU,
		MODBUS_TRANSFER_TCP,
	}MODBUS_TRANSFER_T;


	class CModbusMaster{
	public:
		CModbusMaster();
		virtual ~CModbusMaster();

		int setSlaveId(int nId);
		int getSlaveId();

		int setTransferMode(MODBUS_TRANSFER_T mode);
		MODBUS_TRANSFER_T getTransferMode();

		int packageReadCoils(int nAddress/*in*/, int nCount/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/);
		
		int packageReadRegisters(int nAddress/*in*/, int nCount/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/);
		
		int packageReadFileRecord(int nFileNo/*in*/, int nRecordNo/*in*/, int nRecordSize/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*out*/);

		int packageWriteSingleCoil(int nAddress/*in*/, bool bOn/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/);

		int packageWriteSingleRegister(int nAddress/*in*/, int nValue/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/);
		
		int packageWriteCoils(int nAddress/*in*/, const vector<int>& vecValue/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/);
		
		int packageWriteRegisters(int nAddress/*in*/, const vector<int>& vecValue/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/);
		
		int packageWriteFileRecord(int nFileNo/*in*/, int nRecordNo/*in*/, int nReferType/*in*/, const unsigned char *pData/*in*/, int nDataSize/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/);
		

		int unpackageReadCoils(unsigned char *pData/*in*/, int nDataSize/*in*/, int nCoilsCount/*in*/, vector<int>& vecValue/*out*/);

		int unpackageReadRegisters(unsigned char *pData/*in*/, int nDataSize/*in*/, vector<int>& vecValue/*out*/);
		/*unpackage read file record*/
		int unpakageReadFileRecord(unsigned char *pData/*in*/, int nDataSize/*in*/,  unsigned char *pBuffer/*out*/, int nMaxBufferSize/*in*/, int& nBufferSize/*out*/);

		int unpackageWriteSingleCoil(unsigned char *pData/*in*/, int nDataSize/*in*/, int& nAddress/*out*/, bool &bOn/*out*/);

		int unpackageWriteSingleRegister(unsigned char *pData/*in*/, int nDataSize/*in*/, int &nAddress/*out*/, int &nValue/*out*/);

		int unpackageWriteCoils(unsigned char *pData/*in*/, int nDataSize/*in*/, int& nStartAdress/*out*/, int& nCoilsCount/*out*/);
		
		int unpackageWriteRegisters(unsigned char *pData/*in*/, int nDataSize/*in*/, int& nStartAdress/*out*/, int& nRegisterCount/*out*/);
		/*unpackage write file record*/
		int unpakageWriteFileRecord(unsigned char *pData/*in*/, int nDataSize/*in*/, int nFileNo/*in*/, int nRecordNo/*in*/, int nReferType/*in*/, unsigned char *pBuffer/*out*/, int nMaxBufferSize, int& nBufferSize/*out*/);
		
		int findReplyPackage(unsigned char *pData/*in*/, int nDataSize/*in*/, int& nPackageSize/*out*/);

		MODBUS_TRANSFER_T getMode();

	protected:

		int packageRequest(unsigned char *pPduData/*in*/, int nPduSize/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/);
		int checkReplyPackage(unsigned char *pData/*in*/, int nDataSize/*in*/, int& nFuncCode/*out*/, int& nPduSize/*out*/);
		int cal_crc16(unsigned char *pData, int nSize);
		CModbusPdu* m_pModbusPdu;
	protected:
		int m_nOffset;
		int m_nSalveId;
		MODBUS_TRANSFER_T m_transferMode;
		int m_nSessionId;
		
		CMutexLock m_mutex;
	private:
		CModbusPdu m_modbusPdu;
	};

}
#endif //MODBUS_MASTER_H__20190408
