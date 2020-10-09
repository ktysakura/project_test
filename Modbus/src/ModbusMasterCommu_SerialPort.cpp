#include "ModbusMasterCommu_SerialPort.h"
#pragma comment(lib, "Comm")
/* construct
**/
CModbusMasterCommu_SerialPort::CModbusMasterCommu_SerialPort(){

}

/* destruct
**/
CModbusMasterCommu_SerialPort::~CModbusMasterCommu_SerialPort(){
	this->close();
}

/* open
**/
int CModbusMasterCommu_SerialPort::open(const string& sDevice, BaudRate baudRate, DataBits dataBits, StopBits stopBits, Parity parity){
	return m_serialPort.open(sDevice, baudRate, dataBits, stopBits, parity);
}

int CModbusMasterCommu_SerialPort::setCommInfo(BaudRate baudRate, DataBits dataBits, StopBits stopBits, Parity parity)
{
	return m_serialPort.setCommInfo(baudRate, dataBits, stopBits, parity);
}
/* flush
**/
int CModbusMasterCommu_SerialPort::flush(){
	return 	m_serialPort.flush();
}

/* close
**/
int CModbusMasterCommu_SerialPort::close(){
	return m_serialPort.close();
}

/* do commu
**/

int CModbusMasterCommu_SerialPort::sendAndRecv(
        unsigned char *pSendData, int nDataSize, unsigned char *pRecvBuffer, int nBufferSize, int nHopeSize, int nTimeout){
	
	int i,WriteBytes,nReadBytes,nReadTotal = 0;

	if ((NULL == pSendData) || (nDataSize <= 0) || (NULL == pRecvBuffer) || (nBufferSize <= 0) || (nBufferSize < nHopeSize)){
		return -1;
	}

	if (nDataSize != (WriteBytes = m_serialPort.write(pSendData, nDataSize, nTimeout))){
		return -2;
	}

	nTimeout = nTimeout / 16;
	for (i = 0; i < 16; i++){
		nReadBytes = m_serialPort.read(pRecvBuffer + nReadTotal, nHopeSize - nReadTotal, nTimeout);
		if (nReadBytes < 0){
			printf("m_serialPort.read error %d\n", nReadBytes);
			return -3;
		}

		if(0 == nReadBytes){
			 continue;
		}

		nReadTotal += nReadBytes;
		if (nReadTotal >= 5){
			if (pRecvBuffer[1] & 0x80){
				return nReadTotal;
			}

			if (nReadTotal >= nHopeSize){
//				printf("doCommu i = %d\n", i);
				return nReadTotal;
			}
		}
	}

	if(nReadTotal > 0){
		//stp_mem(pRecvBuffer, nReadTotal);
	}	

	return -4;
}

/* is opened
**/
bool CModbusMasterCommu_SerialPort::isOpened(){
	return m_serialPort.isOpened();
}
