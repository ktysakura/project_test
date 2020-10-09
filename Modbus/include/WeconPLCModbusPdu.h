#pragma once
#include "ModbusPdu.h"
#include "WeconPLCModbusDefine.h"

namespace cbl{

	class CWeconPLCModbusPdu : public CModbusPdu
	{
	public:

		CWeconPLCModbusPdu(void);

		~CWeconPLCModbusPdu(void);
		int packWriteBitInRegisterRequest(int nAddress, int bit, bool bOn, unsigned char *pBuffer, int nBufferSize);
		int dataWithWriteBitInRegisterReply(unsigned char *pData, int nDataSize, int nAddress, int bit, bool bOn);

		int packageReadModuleRequest(int nModuleNo/*in*/, int nAddress/*in*/, int nCount/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/);
		int dataReadModuleReply(unsigned char *pData/*in*/, int nDataSize/*in*/, int nModuleNo/*in*/, int nCount/*in*/, vector<int>& vecValue/*out*/);

		int packageWriteModuleRequest(int nModuleNo/*in*/, int nAddress/*in*/, const vector<int>& vecValue/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/);
		int dataWriteModuleReply(unsigned char *pData/*in*/, int nDataSize/*in*/, int nModuleNo/*in*/, int nAddress/*in*/, int nCount/*in*/);
		
		int packageReadInteriorRequest(int nAddress, int nCount, unsigned char* pBuffer, int nBufferSize);
		int dataWithReadInteriorReply(unsigned char *pData, int nDataSize, int nAddress, int nCount, vector<int> &vecValue);

		int packageOperatePasswordRequest(int nSubCode, unsigned char cKeyType, const string& sPassword, const string& sKeySeed,  unsigned char* pBuffer, int nBufferSize);
		int dataWithOperatePasswordReply(unsigned char* pData, int nDataSize, int nSubCode, int &nCode);

		int packageReadPLCInfoRequest(int nSubCode, int nIndex, int nCount, unsigned char* pBuffer, int nBufferSize);
		int dataWithReadPLCInfoReply(unsigned char *pData/*in*/, int nDataSize/*in*/, int nSubCode, int nCount, string &sResponse);

		int packageRemoveDecTemporaryRequest(const vector<char>& vecKey, unsigned char* pBuffer, int nBuffersize);
		int dataWithRemoveDecTemporaryReply(unsigned char *pData/*in*/, int nDataSize/*in*/, const vector<char>& vecKey);

		int packageWriteInteriorRequest(int nAddress/*in*/, const vector<int>& vecValue/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/);
		int dataWithWriteInteriorReply(unsigned char *pData/*in*/, int nDataSize/*in*/, int nAddress/*in*/, int nCount/*in*/);
		
		int packageWriteSubCodeRequest(int nSubCode/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/);
		int dataWriteSubCodeReply(unsigned char *pData/*in*/, int nDataSize/*in*/, int& nSubCode);
		
		int dataWriteFileRecordReplyEx(unsigned char *pData/*in*/, int nDataSize/*in*/, int nFileNo/*in*/, int nRecordNo/*in*/, int nReferType/*in*/);
		int cal_crc16(unsigned char *pData, int nSize) const;
	protected:
		virtual int checkUserFunctionCode(MODBUS_REPLY_HEADER_T& header , unsigned char* pData) const override;
		
	};

}