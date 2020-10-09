#include "SerialPort.h"
#include "../base/str.h"

using namespace cbl;

#define DEFAULT_READ_TIMEOUT 	5
#define DEFAULT_WRITE_TIMEOUT 	5

/* construct
**/
CSerialPort::CSerialPort()
{
	m_hCom = INVALID_HANDLE_VALUE;
	m_nReadTimeout 	= DEFAULT_READ_TIMEOUT;
	m_nWriteTimeout = DEFAULT_WRITE_TIMEOUT;
}

/* destruct
**/
CSerialPort::~CSerialPort()
{
	this->close();
}

int CSerialPort::getComDevice(int nComIndex, string& sDevice)
{
	if (nComIndex < 0) {
		return -1;
	}

	sDevice = "\\\\.\\COM" + format("%d", nComIndex);

	return 0;
}

/* set commu state
**/
int CSerialPort::setCommState(HANDLE hCom, int nBandRate, int nDataBits, int nStopBits, PARITY_MODE_T parityMode)
{
	DCB dcb;

	/* check params */
	if (NULL == hCom) {
		return -1;
	}

	/* get com state */
	if (TRUE != GetCommState(hCom, &dcb)) {
		return -2;
	}

	/* set band rate */
	switch (nBandRate) {
		case 9600:
		case 19200:
		case 38400:
		case 57600:
		case 115200:
			dcb.BaudRate = nBandRate;
			break;

		default:
			return -3;
	}

	/* set data bits */
	switch (nDataBits) {
		case 7:
		case 8:
			dcb.ByteSize = nDataBits;
			break;

		default:
			return -4;
	}

	/* set stop bits */
	switch (nStopBits) {
		case 1:
			dcb.StopBits = ONESTOPBIT;
			break;

		case 2:
			dcb.StopBits = TWOSTOPBITS;
			break;

		default:
			return -5;
	}

	/* set parity mode */
	switch (parityMode) {
		case PARITY_MODE_NONE:
			dcb.Parity = NOPARITY;
			dcb.fParity = 0;
			break;
		
		case PARITY_MODE_ODD:
			dcb.Parity = ODDPARITY;
			dcb.fParity = 1;
			break;
		
		case PARITY_MODE_EVEN:
			dcb.Parity = EVENPARITY;
			dcb.fParity = 1;
			break;
		
		default:
			return -6;
	}

	/* set com state */
	if (TRUE != SetCommState(hCom, &dcb)) {
		return -7;
	}

	return 0;
}

/* open
**/
int CSerialPort::open(const string& sDevice, int nBandRate, int nDataBits, int nStopBits, PARITY_MODE_T parityMode)
{
	int nExitCode = 0;
	COMMTIMEOUTS timeouts;	
	HANDLE hCom = INVALID_HANDLE_VALUE;

	/* open com device */
	hCom = CreateFileA(sDevice.c_str(), GENERIC_READ | GENERIC_WRITE, 0,
					NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hCom) {
		return -1;
	}

	/* set com stat */
	if (this->setCommState(hCom, nBandRate, nDataBits, nStopBits, parityMode) < 0) {
		nExitCode = -2;
		goto __EXIT_OPEN;
	}

	/* set com buffer */
	if (TRUE != SetupComm(hCom, 4096, 4096)) {
		nExitCode = -3;
		goto __EXIT_OPEN;
	}

	/* set com timeouts */
	timeouts.ReadIntervalTimeout = m_nReadTimeout;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = m_nReadTimeout;

	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = m_nWriteTimeout;
	if (TRUE != SetCommTimeouts(hCom, &timeouts)) {
		nExitCode = -4;
		goto __EXIT_OPEN;	
	}

	/* clear io buffer */
	PurgeComm(hCom, PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT);

__EXIT_OPEN:
	if (0 == nExitCode) {
		m_hCom = hCom;
	} else {
		if (INVALID_HANDLE_VALUE != hCom) {
			CloseHandle(hCom);
		}
	}
	
	return 0;
}

/* set timeout
**/
int CSerialPort::setTimeout(int nReadTimeout, int nWriteTimeout)
{
	COMMTIMEOUTS timeouts;	
	
	/* check params */
	if (INVALID_HANDLE_VALUE == m_hCom) {
		return -1;
	}
	
	/* set read and write timeout */
	timeouts.ReadIntervalTimeout = m_nReadTimeout;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = m_nReadTimeout;

	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = m_nWriteTimeout;
	if (TRUE != SetCommTimeouts(m_hCom, &timeouts)) {
		return -2;	
	}

	/* save data */
	m_nReadTimeout = nReadTimeout;
	m_nWriteTimeout = nWriteTimeout;

	return 0;	
}

/* read
**/
int CSerialPort::read(unsigned char *pData, int nSize)
{
	DWORD dwReadBytes;

	/* check handle */
	if ((NULL == pData) || (nSize <= 0) || (INVALID_HANDLE_VALUE == m_hCom)) {
		return -1;
	}

	/* read file */
	if (TRUE != ReadFile(m_hCom, pData, nSize, &dwReadBytes, NULL)) {
		return -2;
	}

	return dwReadBytes;
}

/* write
**/
int CSerialPort::write(unsigned char *pData, int nSize)
{
	DWORD dwWriteBytes = 0;

	/* check handle */
	if ((NULL == pData) || (nSize <= 0) || (INVALID_HANDLE_VALUE == m_hCom)) {
		return -1;
	}

	/* write file */
	if (TRUE != WriteFile(m_hCom, pData, nSize, &dwWriteBytes, NULL)) {
		return -2;
	}

	return dwWriteBytes;
}

/* flush
**/
int CSerialPort::flush()
{
	if (INVALID_HANDLE_VALUE == m_hCom) {
		return -1;
	}
    PurgeComm(m_hCom, PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT);
	return 0;
}

/* close
**/
int CSerialPort::close()
{
	if (INVALID_HANDLE_VALUE != m_hCom) {
		CloseHandle(m_hCom);
		m_hCom = INVALID_HANDLE_VALUE;
	}

	return 0;
}

/* is opened
**/
bool CSerialPort::isOpened()
{
	return (INVALID_HANDLE_VALUE != m_hCom);
}

/* purge
**/
int CSerialPort::purge()
{
	if (INVALID_HANDLE_VALUE != m_hCom) {
		return PurgeComm(m_hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
	}
	return 0;
}
