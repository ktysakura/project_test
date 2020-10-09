#pragma once
#include "ModbusMaster.h"
#include "WeconPLCModbusPdu.h"

using namespace cbl;
class CWeconPLCMobusMaster : public CModbusMaster
{
public:
	CWeconPLCMobusMaster();
	CWeconPLCMobusMaster(CWeconPLCModbusPdu* pModbusPdu);

	virtual ~CWeconPLCMobusMaster();
	int packWriteBitInRegister(int nAddress, int bit, bool bOn, unsigned char *pBuffer, int nBufferSize);
	int unpackWriteBitInRegister(unsigned char *pData/*in*/, int nDataSize/*in*/, int nAddress, int bit, bool bOn);
	
	int packageReadModule(int nModuleNo/*in*/, int nAddress/*in*/, int nCount/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/);
	int unpackageReadModule(unsigned char *pData/*in*/, int nDataSize/*in*/, int nModuleNo/*in*/, int nCount, vector<int>& vecValue/*out*/);

	int packageWriteModule(int nModuleNo, int nAddress, const vector<int>& vecValue/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/);
	int unpackageWriteModule(unsigned char *pData/*in*/, int nDataSize/*in*/, int nModuleNo, int nAddress, int nCount);
	
	int packageMutliSubCode(int nSubCode/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/);
	int unpackageMutliSubCode(unsigned char *pData/*in*/, int nDataSize/*in*/, int& nSubCode/*in*/);
	
	int packageWriteFileRecordEx(int nFileNo/*in*/, int nRecordNo/*in*/, int nReferType/*in*/, const unsigned char *pData/*in*/, int nDataSize/*in*/, unsigned char *pBuffer/*out*/, int nBufferSize/*in*/);
	int unpakageWriteFileRecordEx(unsigned char *pData/*in*/, int nDataSize/*in*/, int nFileNo/*in*/, int nRecordNo/*in*/, int nReferType/*in*/);

	int packageOperatePassword(int nSubCode, unsigned char cKeyType, const string& sPassword, const string& sKeySeed, unsigned char* pBuffer, int nBufferSize);
	int unpackageOperatePassword(unsigned char *pData/*in*/, int nDataSize/*in*/,int nSubCode, int &nCode);

	int packageReadPLCInfo(int nSubCode, int nIndex, int nCount, unsigned char* pBuffer, int nBufferSize);
	int unpackageReadPLCInfo(unsigned char *pData/*in*/, int nDataSize/*in*/, int nSubCode, int nCount, string& sData);
	
	int packageRemoveDecTemporary(const vector<char>& vecKey, unsigned char* pBuffer, int nBufferSize);
	int unpackageRemoveDecTemporary(unsigned char* pData, int nDataSize, const vector<char> &vecKey);

	int packageReadInterior(int nAddress, int nCount, unsigned char* pBuffer, int nBufferSize);
	int unpackageReadInterior(unsigned char *pData/*in*/, int nDataSize/*in*/, int nAddress, int nCount, vector<int>& vecValue);

	int packageWriteInterior(int nAddress, const vector<int>& vecValue, unsigned char* pBuffer, int nBufferSize);
	int unpackageWriteInterior(unsigned char *pData/*in*/, int nDataSize/*in*/, int nAddress, int nCount);
	
	int getRecvSize(int nCode, int nCount, int nSubCode = -1);
	int getSubRecvSize(int nCode, int nSubCode, int nCount = 0);
private:
	CWeconPLCModbusPdu m_weconPLCModbusPdu;

};
