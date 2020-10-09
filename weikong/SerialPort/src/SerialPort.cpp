#include "SerialPort.h"
#ifndef _WIN32
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <sys/ioctl.h>
	#include <linux/serial.h>
	#include <termios.h>
#endif //_WIN32

#define DEFAULT_READ_TIMEOUT 	5
#define DEFAULT_WRITE_TIMEOUT 	5

namespace cbl{
/* construct
**/
CSerialPort::CSerialPort(){
	
#ifdef _WIN32
	m_hCom = INVALID_HANDLE_VALUE;
#else //_WIN32
	m_fd = -1;
#endif //_WIN32

	m_nReadTimeout 	= DEFAULT_READ_TIMEOUT;
	m_nWriteTimeout = DEFAULT_WRITE_TIMEOUT;
}

/* destruct
**/
CSerialPort::~CSerialPort(){
	this->close();
}

#ifdef _WIN32

int CSerialPort::getComDevice(int nComIndex, string& sDevice){

	if (nComIndex < 0){
		return -1;
	}
//	sDevice = "\\\\.\\COM" + to_string(nComIndex);

	return 0;
}

/* set commu state
**/
int CSerialPort::setCommState(HANDLE hCom, BaudRate baudRate, DataBits dataBits, StopBits stopBits, Parity parity){

	DCB dcb;

	/* check params */
	if (NULL == hCom){
		return -1;
	}

	/* get com state */
	if (TRUE != GetCommState(hCom, &dcb)){
		return -2;
	}

	/* set band rate */
	dcb.BaudRate = baudRate;

	/* set data bits */
	switch (dataBits){
	case 7:
	case 8:{
		dcb.ByteSize = dataBits;
		break;
	}
	default:
		return -4;
	}

	/* set stop bits */
	dcb.StopBits = stopBits;

	/* set parity mode */
	dcb.Parity = parity;
	dcb.fParity = (dcb.Parity  != ParityNone); 

	/* set com state */
	if (TRUE != SetCommState(hCom, &dcb)){
		return -5;
	}

	return 0;
}

#else //_WIN32

int CSerialPort::setCommState(int fd, BaudRate bandRate, DataBits dataBits, StopBits stopBits, Parity parity){
	
	struct termios attr;
	
	/* check params */
	if(fd < 0){
		return -1;
	}
	
	/* get attr */
	if (tcgetattr(fd, &attr) < 0){
		return -2;
	}
	
	/* set default attr */
	attr.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG | ECHOE); 
	attr.c_iflag &= ~(ICRNL | IXON | BRKINT | INLCR |IGNCR);
	attr.c_oflag &= ~OPOST;
	attr.c_cflag = (CLOCAL | CREAD);
	
	/* set band rate */
	switch (bandRate){
	case 9600:{
		attr.c_cflag |= B9600;
		break;
	}
	case 19200:{
		attr.c_cflag |= B19200;
		break;
	}
	case 38400:{
		attr.c_cflag |= B38400;
		break;
	}
	case 57600:{
		attr.c_cflag |= B57600;
		break;
	}
	case 115200:{
		attr.c_cflag |= B115200;
		break;
	}
	default:
		return -3;
	}
	
	/* set data bits */
	attr.c_cflag &= ~CSIZE;  
	switch (dataBits){  
    case 7: 
        attr.c_cflag |= CS7;  
        break;  
    case 8:  
        attr.c_cflag |= CS8;  
        break;  
    default: 
		return -4;
	}
	
	/* set stop bits */
	switch(stopBits){
		case 1:{
			attr.c_cflag &= ~CSTOPB;
			break;
		}
		case 2:{
			attr.c_cflag |= CSTOPB;
			break;
		}
		default:{
			return -5;
		}
	}
	
	/* set parity bit */
	switch(parity){
		case ParityNone:{
			attr.c_iflag &= ~(INPCK | ISTRIP);
			attr.c_cflag &= ~PARENB;
			break;
		}
		case ParityOdd:
		case ParityEven:{
			attr.c_iflag |= (INPCK | ISTRIP); 
			attr.c_cflag |= PARENB;
			if(PARITY_MODE_ODD == parityMode){
				attr.c_cflag |= PARODD;
			}
			else{
				attr.c_cflag &= ~PARODD;
			}
			break;
		}
		default:{
			return -6;
		}
	}
	
	/* set control char */	
     attr.c_cc[VTIME]  = 1;
     attr.c_cc[VMIN] = 1;

	/* set attr */	
	if(tcsetattr(fd, TCSANOW, &attr) < 0){
		return -7;
	}
	
	return 0;
}
#endif //_WIN32

/* open
**/
int CSerialPort::open(const string& sDevice, BaudRate baudRate, DataBits dataBits, StopBits stopBits, Parity parity){
	
#ifdef _WIN32

	int nExitCode = 0;
	COMMTIMEOUTS timeouts;	
	HANDLE hCom = INVALID_HANDLE_VALUE;
	string sComm = "\\\\.\\" + sDevice;
	/* open com device */
	if (INVALID_HANDLE_VALUE == (hCom = CreateFileA(sComm.c_str(), GENERIC_READ | GENERIC_WRITE, 0,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))){
		return -1;
	}

	/* set com stat */
	if (this->setCommState(hCom, baudRate, dataBits, stopBits, parity) < 0){
		nExitCode = -2;
		goto __EXIT_OPEN;
	}

	/* set com buffer */
	if (TRUE != SetupComm(hCom, 4096, 4096)){
		nExitCode = -3;
		goto __EXIT_OPEN;
	}

	/* set com timeouts */
	timeouts.ReadIntervalTimeout = m_nReadTimeout;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = m_nReadTimeout;

	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = m_nWriteTimeout;
	if(TRUE != SetCommTimeouts(hCom, &timeouts)){
		nExitCode = -4;
		goto __EXIT_OPEN;	
	}

__EXIT_OPEN:

	/* exit open */
	if (0 == nExitCode){
		m_hCom = hCom;
		/* clear io buffer */
		this->flush();
	}
	else{
		if (INVALID_HANDLE_VALUE != hCom){
			CloseHandle(hCom);
		}
	}
	
	return nExitCode;
	
#else //_WIN32

	int fd = -1;
	
	/* open device */
	if ((fd = ::open (sDevice.c_str (), O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY)) < 0){
		return -1;		
	}
	
	/* set com state */
	if (this->setCommState(fd, baudRate, dataBits, stopBits, parity) < 0){
		::close(fd);
		return -2;	
	}
		
	/* save fd */
	m_fd = fd;

	/* flush */
	this->flush();

	return 0;
	
#endif //_WIN32
}

/* set timeout
**/
int CSerialPort::setTimeout(int nReadTimeout,int nWriteTimeout){
	
#ifdef _WIN32

	COMMTIMEOUTS timeouts;	
	
	/* check params */
	if(INVALID_HANDLE_VALUE == m_hCom){
		return -1;
	}
	
	/* set read and write timeout */
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = nReadTimeout;

	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = nWriteTimeout;
	if(TRUE != SetCommTimeouts(m_hCom, &timeouts)){
		return -2;	
	}
	
#else //_WIN32
	if(m_fd < 0){
		return -1;
	}
#endif //_WIN32

	/* save data */
	m_nReadTimeout = nReadTimeout;
	m_nWriteTimeout = nWriteTimeout;

	return 0;	
}
int CSerialPort::setReadTimeout(int nReadTimeout)
{
	if (m_nReadTimeout != nReadTimeout)
	{
		return setTimeout(nReadTimeout, m_nWriteTimeout);
	}

	return 0;
}
int CSerialPort::setWriteTimeout(int nWriteTimeout)
{
	if (m_nWriteTimeout != nWriteTimeout)
	{
		return setTimeout(m_nReadTimeout, nWriteTimeout);
	}

	return 0;
}
int CSerialPort::getReadTimeout()
{
	return m_nReadTimeout;
}

int CSerialPort::getWriteTimeout()
{
	return m_nWriteTimeout;
}

int CSerialPort::setCommInfo(BaudRate baudRate, DataBits dataBits, StopBits stopbits, Parity parity)
{
#ifdef _WIN32
	return this->setCommState(m_hCom, baudRate, dataBits, stopbits, parity);
#else
	return this->setCommState(m_fd, baudRate, dataBits, stopbits, parity);
#endif
}
/* read
**/
int CSerialPort::read(unsigned char *pData, int nSize, int nTimeout){

	setReadTimeout(nTimeout);
#ifdef _WIN32

	DWORD dwReadBytes;

	/* check handle */
	if ((NULL == pData) || (nSize <= 0) || (INVALID_HANDLE_VALUE == m_hCom)){
		return -1;
	}

	/* read file */
	if (TRUE != ReadFile(m_hCom, pData, nSize, &dwReadBytes, NULL)){
		return -2;
	}

	return dwReadBytes;
	
#else //_WIN32

	fd_set rfds;
	int nRtn,nReadBytes;
	struct timeval tv = {m_nReadTimeout / 1000, (m_nReadTimeout % 1000) * 1000};
	
	/* check params */
	if ((NULL == pData) || (nSize <= 0) || (m_fd <= 0)){
		return -1;	
	}
		
	/* select fds */
	FD_ZERO (&rfds);
	FD_SET (m_fd, &rfds);
	if ((nRtn = select (m_fd + 1, &rfds, NULL, NULL, &tv)) < 0){
        return -2;
	}
	if(0 == nRtn){
		return 0;
	}
	
	/* read data */
	if ((nReadBytes = ::read(m_fd, pData, nSize)) < 0){
        return -3;
	}
	
	return nReadBytes;
	
#endif //_WIN32
}

/* write
**/
int CSerialPort::write(unsigned char *pData, int nSize, int nTimeout){
	
	setWriteTimeout(nTimeout);
#ifdef _WIN32

	DWORD dwWriteBytes = 0;

	/* check handle */
	if ((NULL == pData) || (nSize <= 0) || (INVALID_HANDLE_VALUE == m_hCom)){
		return -1;
	}

	/* write file */
	if (TRUE != WriteFile(m_hCom, pData, nSize, &dwWriteBytes, NULL)){
		printf("WriteFile error, dwWriteBytes = %d(%d)\n", dwWriteBytes, GetLastError());
		return -2;
	}

	return dwWriteBytes;
	
#else //_WIN32
	
	fd_set wfds;
	int nWriteBytes;
	struct timeval tv = {m_nWriteTimeout / 1000, (m_nWriteTimeout % 1000) * 1000};
	
	/* check params */
	if ((NULL == pData) || (nSize <= 0) || (m_fd <= 0)){
		return -1;	
	}
	
	/* select fds */
	FD_ZERO (&wfds);
	FD_SET (m_fd, &wfds);
	if (select (m_fd + 1, NULL, &wfds, NULL, &tv) <0){
		return -2;
	}
	
	if((nWriteBytes = ::write (m_fd, pData, nSize)) < 0){
		return -3;
	}
	
	return nWriteBytes;
	
#endif //0
}

/* flush
**/
int CSerialPort::flush(){

#ifdef _WIN32
	if(INVALID_HANDLE_VALUE == m_hCom){
		return -1;
	}
    PurgeComm(m_hCom, PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT);
#else //_WIN32
	if(m_fd < 0){
		return -1;	
	}
	tcflush (m_fd, TCIOFLUSH);
#endif //_WIN32
	
	return 0;
}

/* close
**/
int CSerialPort::close(){

#ifdef _WIN32
	if (INVALID_HANDLE_VALUE != m_hCom){
		CloseHandle(m_hCom);
		m_hCom = INVALID_HANDLE_VALUE;
	}
#else //_WIN32
	if (m_fd >= 0){
		::close (m_fd);
		m_fd = -1;
	}
#endif //_WIN32

	return 0;
}

/* is opened
**/
bool CSerialPort::isOpened(){

#ifdef _WIN32
	return (INVALID_HANDLE_VALUE != m_hCom);
#else //_WIN32
	return (m_fd >= 0);
#endif //_WIN32

}


}