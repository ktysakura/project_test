#if !defined(RTU_MODBUS_MASTER_COMMU_H__20190326)
#define RTU_MODBUS_MASTER_COMMU_H__20190326

#include "WeconPLCMobusMaster.h"

using namespace cbl;
class CModbusMasterCommu{
public:
	CModbusMasterCommu();
	virtual ~CModbusMasterCommu();

    int setSlaveId(int nId);
	int setTransferMode(MODBUS_TRANSFER_T mode);
	MODBUS_TRANSFER_T getTransferMode();
	
	int readSingleCoil(int nAddress, bool& bOn, int nTimeout = 1000);
    int readCoils(int nAddress, int nCount, vector<int>& vecValue, int nTimeout = 1000);
	int readSingleRegister(int nAddress, int& nValue, int nTimeout = 1000);

    int readRegisters(int nAddress, int nCount, vector<int>& vecValue, int nTimeout = 1000);
	/*read Multi Coils  vecValue.size() > 0x07D0 = 2000*/
	int readMultiCoils(int nAddress, int nCount, vector<int>& vecValue, int nTimeout = 1000);
	/*read Multi registers  vecValue.size() > 0x007D 125*/
	int readMultiRegisters(int nAddress, int nCount, vector<int>& vecValue, int nTimeout = 1000);

    int writeSingleCoil(int nAddress, int nValue, int nTimeout = 1000);
    int writeSingleRegister(int nAddress,  int nValue, int nTimeout = 1000);

    int writeCoils(int nAddress, const vector<int>& vecValue, int nTimeout = 1000);
	int writeMultiCoils(int nAddress, const vector<int>& vecValue, int nTimeout = 1000);
	/*write registers  vecValue.size() <= 0x00FB*/
    int writeRegisters(int nAddress, const vector<int>& vecValue, int nTimeout = 1000);
	/*write registers  vecValue.size() > 0x00FB*/
	int writeMultiRegisters(int nAddress, const vector<int>& vecValue, int nTimeout = 1000);
protected:
	int writeFileRecord(int nFileNo/*in*/, int nRecordNo/*in*/, unsigned char *pBuffer/*in*/, int nBufferSize/*in*/, int nTimeout = 1000/*in*/);
	int readFileRecord(int nFileNo/*in*/, int nRecordNo/*in*/, int nRecordSize/*in*/, unsigned char* pFileBuffer/*out*/, int nMaxBufferSize/*in*/, int nTimeout = 1000/*in*/);
public:
	
	virtual int flush() = 0;
	virtual int close() = 0;
	virtual bool isOpened() = 0;
	
protected:
	int onceAllocate(int nMultiCount, int nMaxOnceCount, vector<int> &vecVal);
	int findReplyPackage(unsigned char *pData, int nDataSize, int& nPackageSize);
    int doCommu(unsigned char *pSendData/*in*/, int nDataSize/*in*/, unsigned char *pRecvBuffer/*out*/, int nBufferSize/*in*/ ,int nHopeSize/*in*/, int nTimeout/*in*/);
protected:
    virtual int sendAndRecv(unsigned char *pSendData/*in*/, int nDataSize/*in*/, unsigned char *pRecvBuffer/*out*/, int nBufferSize/*in*/ ,int nHopeSize/*in*/, int nTimeout/*in*/) = 0;
protected:
	CWeconPLCMobusMaster m_modbusMaster;
	CMutexLock m_commlock;
};

#endif //RTU_MODBUS_MASTER_COMMU_H__20190326
