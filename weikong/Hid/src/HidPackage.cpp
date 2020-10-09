#include "HidPackage.h"
using namespace std;
USBHID_BEGIN_NAMESPACE

void CHidPackage::setSessionId(unsigned char sessionId)
{
	m_sessionId = sessionId;
}

unsigned char CHidPackage::getSessionId() const
{
	return m_sessionId;
}

void CHidPackage::setSendCount(unsigned char nSendCount)
{
	m_sendCount = nSendCount;
}

int CHidPackage::getTotalCount() const
{
	return m_sendCount  & 0xF;
}
int CHidPackage::getCurrentCount() const
{
	return (m_sendCount >> 4) & 0x0F;
}


void CHidPackage::setAppData(const string &appData)
{
	if (!m_appData.empty()) {
		m_appData.clear();
	}
	m_appData = appData;
}

void CHidPackage::setAppData(const char *appData, size_t length)
{
	if (appData == NULL || length < 0) {
		return;
	}

	if (!m_appData.empty()) {
		m_appData.clear();
	}

	m_appData.assign(appData, length);

}

const string& CHidPackage::getAppData() const
{
	return m_appData;
}

void CHidPackage::setReportLen(int len) 
{
	m_outReportLen = len;
}

int CHidPackage::pack(string &packData)
{
	int nLen, appLen;
	const char *p;
	unsigned char xOR; 
	static const int c_sessionSize = sizeof(m_sessionId), c_sendCountSize = sizeof(m_sendCount), c_xOR = sizeof(xOR);

	if (!packData.empty()) {
		packData.clear();
	}

	nLen = 0;
	appLen = m_appData.size();
	/*设置包缓存区大小*/
	packData.reserve(m_outReportLen);
#ifdef WIN32
	/*windows下hid协议包的第一个字节固定为0x00,否则会发送失败*/
	packData.append(1, '\x00');
	nLen++;
#endif
	/*拼接会话ID*/
	p = (char*)&m_sessionId;
	packData.append(p, c_sessionSize);
	nLen += c_sessionSize;
	/*拼接当前包的(次数/总次数)*/
	p = (char*)&m_sendCount;
	packData.append(p, c_sendCountSize);
	nLen += c_sendCountSize;

	p = m_appData.data();
	packData.append(p, appLen);
	nLen += appLen;
	
	/*hid包写长度必须为outreport的大小，所以需要补足长度*/
	int nPadding = m_outReportLen - nLen - 1;
	if (nPadding > 0) {
		packData.append(nPadding, '\x00');
	}
#ifdef WIN32
	/*亦或校验*/
	xOR = this->calcul_Xor((unsigned char*)(packData.data() + 1), 63);
#else/*WIN32*/
	xOR = this->calcul_Xor(packData.data(), packData.size());
#endif/*LINUX*/

	p = (char *)&xOR;
	packData.append(p, c_xOR);
	nLen += c_xOR;

	return packData.size();
}

int CHidPackage::pack(unsigned char *packData, int *dataLen)
{
	int ret;
	string data;
	
	if ( packData == NULL || dataLen == NULL)
	{
		return -1;
	}
	
	if ((ret = this->pack(data)) < 0)
	{
		return -2;
	}

	*dataLen = data.size();
	memcpy(packData, data.data(), *dataLen);

	return *dataLen;
}

int CHidPackage::unpack(const string &unPackData)
{
	return this->unpack((const unsigned char *)unPackData.data(), unPackData.size());
}

int CHidPackage::unpack(const unsigned char *unPackData, int dataLen)
{
	int nLen;
	unsigned char xOR;

	if (unPackData == NULL) {
		return -1;
	}

	if (dataLen < PACKAGE_HEAD_LEN + PACKAGE_TAIL_LEN) {
		return -2;
	}

#ifdef WIN32
	xOR = this->calcul_Xor(unPackData, m_outReportLen - 1 - PACKAGE_TAIL_LEN);
#else
	xOR = this->calcul_Xor(unPackData, m_outReportLen  - PACKAGE_TAIL_LEN);
#endif
	/*异或校验*/
	if (unPackData[dataLen - sizeof(xOR)] != xOR) {
		return -3;
	}

	nLen = 0;
	memcpy(&m_sessionId, &unPackData[nLen], sizeof(m_sessionId));
	nLen += sizeof(m_sessionId);

	memcpy(&m_sendCount, &unPackData[nLen], sizeof(m_sendCount));
	nLen += sizeof(m_sendCount);

	m_appData.assign((char*)(&unPackData[nLen]), m_outReportLen - 1 - PACKAGE_HEAD_LEN - PACKAGE_TAIL_LEN);

	return 0;
}

unsigned char CHidPackage::calcul_Xor(const unsigned char *pbuf, int len)
{
	int i;
	unsigned char temp = 0xff;
	for (i = 0; i < len; i++)
	{
		temp = temp ^ pbuf[i];
	}
	return temp;
}

USBHID_END_NAMESPACE