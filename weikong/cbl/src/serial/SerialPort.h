#if !defined(SERIAL_PORT_H__20190326)
#define SERIAL_PORT_H__20190326

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <windows.h>

using namespace std;

typedef enum {
	PARITY_MODE_NONE,
	PARITY_MODE_ODD,
	PARITY_MODE_EVEN,
} PARITY_MODE_T;

class CSerialPort
{
public:
	CSerialPort();
	virtual ~CSerialPort();

	static int getComDevice(int nComIndex, string& sDevice);
	int open(const string& sDevice, int nBandRate, int nDataBits, int nStopBits, PARITY_MODE_T parityMode);
	int setTimeout(int nReadTimeout, int nWriteTimeout);
	int read(unsigned char *pData, int nSize);
	int write(unsigned char *pData, int nSize);
	int purge();
	int flush();
	int close();

	bool isOpened();

private:
	int setCommState(HANDLE hCom, int nBandRate, int nDataBits, int nStopBits, PARITY_MODE_T parityMode);

private:
	HANDLE m_hCom;
	int m_nReadTimeout;
	int m_nWriteTimeout;
};

#endif //SERIAL_PORT_H__20190326
