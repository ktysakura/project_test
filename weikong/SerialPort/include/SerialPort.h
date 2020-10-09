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
	//打开串口
	int open(const string& sDevice, BaudRate nBandRate, DataBits nDataBits, StopBits nStopBits, Parity parity);
	//获取读取超时时间
	int getReadTimeout();
	//获取写入超时时间
	int getWriteTimeout();
	//设置串口参数
	int setCommInfo(BaudRate baudRate, DataBits dataBits, StopBits stopbits, Parity parity);
	//设置串口读写缓存区大小
	int setCommBuffer(unsigned long nInQuque, unsigned long nOutQuque);
	//读取数据
	int read(unsigned char *pData, int nSize, int nTimeout);
	//写入数据
	int write(unsigned char *pData, int nSize, int nTimeout);
	//清空缓冲区
	int flush();
	//关闭串口
	int close();
	bool isOpened();
private:
	//设置超时事件
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
