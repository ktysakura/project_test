#if !defined(RTU_MODBUS_MASTER_COMMU_SERIAL_PORT_H__20190326)
#define RTU_MODBUS_MASTER_COMMU_SERIAL_PORT_H__20190326

#include "WeconPLCModbusMasterCommu.h"
#include "SerialPort.h"

class CModbusMasterCommu_SerialPort : public CWeconPLCModbusMasterCommu
{

public:
	CModbusMasterCommu_SerialPort();
	virtual ~CModbusMasterCommu_SerialPort();

	int open(const string& sDevice, BaudRate baudRate, DataBits dataBits, StopBits stopBits, Parity parity);

	int setCommInfo(BaudRate baudRate, DataBits dataBits, StopBits stopBits, Parity parity);

	int setTimeout(int nReadTimeout,int nWriteTimeout);

public:
	virtual int flush() override;

	virtual int close() override;

	virtual bool isOpened() override;

private:
    int sendAndRecv(
            unsigned char *pSendData, int nDataSize, unsigned char *pRecvBuffer, int nBufferSize, int nHopeSize, int nTimeout);
private:
	CSerialPort m_serialPort;
};

#endif //RTU_MODBUS_MASTER_COMMU_SERIAL_PORT_H__20190326
