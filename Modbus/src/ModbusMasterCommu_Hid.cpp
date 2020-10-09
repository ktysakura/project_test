#include "ModbusMasterCommu_Hid.h"
#pragma comment(lib, "Comm")
#include <algorithm>
CModbusMasterCommu_Hid::CModbusMasterCommu_Hid(void)
{
}

CModbusMasterCommu_Hid::~CModbusMasterCommu_Hid(void)
{
}

int CModbusMasterCommu_Hid::open(unsigned short vid, unsigned short pid, const char* szPath, const wchar_t *serial_number)
{
	int nRet = 0;

	if ((nRet = m_hidChannel.open(vid, pid, szPath, serial_number)) < 0)
	{
		return -1;
	}

	return 0;
}

int CModbusMasterCommu_Hid::flush()
{
	return 0;	
}
int CModbusMasterCommu_Hid::close()
{
	return m_hidChannel.close();
}

bool CModbusMasterCommu_Hid::isOpened()
{
	return m_hidChannel.isOpen();
}

int  CModbusMasterCommu_Hid::sendAndRecv(unsigned char *pSendData/*in*/, int nDataSize/*in*/, unsigned char *pRecvBuffer/*out*/, int nBufferSize/*in*/ ,int nHopeSize/*in*/, int nTimeout/*in*/)
{
	int nReceiveSize;
	string response;

	if ((nReceiveSize = m_hidChannel.sendAndRecv(pSendData, nDataSize, nHopeSize, response, nTimeout)) <= 0)
	{
		return -1;
	}

	memcpy(pRecvBuffer, response.data(), nReceiveSize);

	return nReceiveSize;
}