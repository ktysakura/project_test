#if !defined(SERIAL_PORT_H__20190326)
#define SERIAL_PORT_H__20190326

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "SerialPortDefine.h"
#ifdef _WIN32
#include <windows.h>
#endif //_WIN32

using namespace std;
namespace cbl{

class CSerialPort{
public:
	CSerialPort();
	virtual ~CSerialPort();

#ifdef _WIN32
	static int getComDevice(int nComIndex,string& sDevice);
#endif //_WIN32
	//�򿪴���
	int open(const string& sDevice, BaudRate nBandRate, DataBits nDataBits, StopBits nStopBits, Parity parity);
	//��ȡ��ȡ��ʱʱ��
	int getReadTimeout();
	//��ȡд�볬ʱʱ��
	int getWriteTimeout();
	//���ô��ڲ���
	int setCommInfo(BaudRate baudRate, DataBits dataBits, StopBits stopbits, Parity parity);
	//���ô��ڶ�д��������С
	int setCommBuffer(unsigned long nInQuque, unsigned long nOutQuque);
	//��ȡ����
	int read(unsigned char *pData, int nSize, int nTimeout);
	//д������
	int write(unsigned char *pData, int nSize, int nTimeout);
	//��ջ�����
	int flush();
	//�رմ���
	int close();
	bool isOpened();
private:
	//���ó�ʱ�¼�
	int setTimeout(int nReadTimeout,int nWriteTimeout);
	int setReadTimeout(int nReadTimeout);
	int setWriteTimeout(int nWriteTimeout);
#ifdef _WIN32
	int setCommState(HANDLE hCom, BaudRate baudRate, DataBits dataBits, StopBits stopBits, Parity parity);
#else //_WIN32
	int setCommState(int fd, BaudRate baudRate, DataBits nDataBits, StopBits nStopBits, Parity parityMode);
#endif //_WIN32

private:
	
#ifdef _WIN32
	HANDLE m_hCom;
#else //_WIN32
	int m_fd;	
#endif //_WIN32

	int m_nReadTimeout,m_nWriteTimeout;
	SERIALPORT_INFO_T m_serialPortInfo;
};

}
#endif //SERIAL_PORT_H__20190326
