#if !defined(RTU_MODBUS_MASTER_COMMU_HID_H__20190326)
#define RTU_MODBUS_MASTER_COMMU_HID_H__20190326

#include "WeconPLCModbusMasterCommu.h"
#include "HidChannel.h"

class CModbusMasterCommu_Hid : public CWeconPLCModbusMasterCommu
{
public:
	CModbusMasterCommu_Hid(void);
	~CModbusMasterCommu_Hid(void);

public:
	int open(unsigned short vid, unsigned short pid, const char* szPath, const wchar_t *serial_number);
	virtual int flush() override;
	virtual int close() override;
	virtual bool isOpened() override;
protected:
	virtual int sendAndRecv(unsigned char *pSendData/*in*/, int nDataSize/*in*/, unsigned char *pRecvBuffer/*out*/, int nBufferSize/*in*/ ,int nHopeSize/*in*/, int nTimeout/*in*/) override;
private:
	usbhid::CHidChannel m_hidChannel;
};

#endif