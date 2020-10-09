#if !defined (MODBUS_PDU_H__20190408)
#define MODBUS_PDU_H__20190408

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "ModbusDefine.h"
#include "BigEndian.h"
using namespace std;

namespace cbl{

	/* define modbus reply header
	**/
	typedef struct{
		int nFuncCode;
		int nErrorCode;
		int nPduSize;
	}MODBUS_REPLY_HEADER_T;
	class CModbusPdu{
	public:
		CModbusPdu();
		virtual ~CModbusPdu();
		int packageReadCoilsRequest(int nAddress, int nCount, unsigned char *pBuffer, int nBufferSize) const;
		int packageReadRegistersRequest(int nAddress, int nCount, unsigned char *pBuffer, int nBufferSize) const;

		int packageWriteSingleCoilRequest(int nAddress, int nValue, unsigned char *pBuffer, int nBufferSize) const;
		int packageWriteSingleRegisterRequest(int nAddress, int nValue, unsigned char *pBuffer, int nBufferSize) const;

		int packageWriteCoilsRequest(int nAddress, const vector<int>& vecValue, unsigned char *pBuffer, int nBufferSize) const;
		int packageWriteRegistersRequest(int nAddress, const vector<int>& vecValue, unsigned char *pBuffer, int nBufferSize) const;

		int packageWriteFileRecordRequest(int nFileNo/*in*/, int nRecordNo/*in*/, int nReferType/*in*/, const unsigned char *pData/*in*/, int nDataSize/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/);
		int packageReadFileRecordRequest(int nFileNo/*in*/, int nRecordNo/*in*/, int nRecordSize/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*out*/);

		int dataReadCoilsReply(unsigned char *pData, int nDataSize, int nCoilsCount, vector<int>& vecValue) const;
		int dataReadRegistersReply(unsigned char *pData, int nDataSize, vector<int>& vecValue) const;
		int dataWriteCoilsReply(unsigned char *pData, int nDataSize, int& nStartAdress, int& nCount) const;
		int dataWriteRegistersReply(unsigned char *pData, int nDataSize, int& nStartAdress, int& nCount) const;
		int dataWriteFileRecordReply(unsigned char *pData/*in*/, int nDataSize/*in*/, int nFileNo/*in*/, int nRecordNo/*in*/, int nReferType/*in*/, unsigned char *pBuffer/*out*/, int nBufferMaxSize /*in*/, int& nBufferSize/*out*/);
		int dataReadFileRecordReply(unsigned char *pData/*in*/, int nDataSize/*in*/,  unsigned char *pBuffer/*out*/, int nMaxBufferSize/*in*/, int& nBufferSize/*out*/);
		int checkPduHeader(unsigned char *pData, int nDataSize, MODBUS_REPLY_HEADER_T& header) const;
	protected:	
		virtual int checkUserFunctionCode(MODBUS_REPLY_HEADER_T& header, unsigned char* pData)const
		{
			return 0;
		}
	};

}
#endif //MODBUS_PDU_H__20190408
