#include "HidChannel.h"
#include "AutoLock.h"

USBHID_BEGIN_NAMESPACE

CHidChannel::CHidChannel(unsigned char initSessionId /*= 0*/)
{
	m_sessionId = initSessionId;
}

CHidChannel::~CHidChannel()
{
	if (this->isOpen())
	{
		this->close();
	}
}

int CHidChannel::open(unsigned short vid, unsigned short pid, const char* szPath, const wchar_t *serial_number /*= NULL*/)
{
	int ret;

	ret = m_hid.open(vid, pid, szPath, serial_number);
	if (ret < 0) {
		return -1;
	}

	m_bOpened = true;
	/*获取报文缓存区大小*/
	m_nOutReportLen = m_hid.get_output_report_length();
	m_nInputReportLen = m_hid.get_input_report_length();
	
	return 0;
}

int CHidChannel::read(unsigned char *pRecv, size_t length, string &sReponse)
{
	int nTotalCount, nCurCount, nSessionID, readLen;
	CHidPackage package;
	
	if (pRecv == NULL) {
		return -1;
	}
	memset(pRecv, 0, length);
	/*先尝试读取第一个包*/
	readLen = m_hid.read(pRecv, length, TIMEOUT_POLL_READ);
	if (readLen <= 0) {
		return readLen;
	}

	/*设置报文长度,*/
	package.setReportLen(length);
	/*拆解hid自定义协议包*/
	if (package.unpack(pRecv, readLen) < 0) {
		return -2;
	}

	nSessionID = package.getSessionId();
	nTotalCount = package.getTotalCount();//读取大包需要的总次数
	nCurCount = package.getCurrentCount();//读取大包当前的次数
	sReponse.append(package.getAppData());
	/*若当前需要接收的总次数大于1,继续接收剩余响应包*/
	while(--nTotalCount)
	{
		memset(pRecv, 0, length);	
		readLen = m_hid.read(pRecv, length, TIMEOUT_POLL_READ);
		if (readLen <= 0) {
			return readLen;
		}

		if (package.unpack(pRecv, readLen) < 0) {
			return -3;
		}
		/** session_id如果不一样,表示hid通讯错误
			如果一样，则表示是在同一大包中
		*/
		if (nSessionID != package.getSessionId()) {
			return -4;
		}
		/*拼接完整的应答包*/
		sReponse.append(package.getAppData());
	}

	return sReponse.size();
}

int CHidChannel::close()
{
	if (this->isOpen()) {
		m_hid.close();
		m_bOpened = false;
	}
	
	return 0;
}

bool CHidChannel::isOpen()
{
	return m_bOpened;
}

unsigned char CHidChannel::nextSessionId(int nCount /*= 1*/)
{
	m_sessionId += nCount;
	if (m_sessionId > 127) {
		m_sessionId = 1;
	}

	return m_sessionId;
}

int CHidChannel::makeRequest(unsigned char *data, size_t length, list<CHidPackage> &listRequest)
{
	CHidPackage package;
	string appData;
	vector<int> vecLen;
	int nAppLen, nRemainLen, i, nSize, nOffset, nSessionId;

	if (data == NULL || length <= 0) {
		return -1;
	}

#if WIN32
	/*计算包的真实长度:64 - 1 - 2 - 1 = 0,第一个字节固定为0x00*/
	nAppLen = m_nOutReportLen - 1 - PACKAGE_HEAD_LEN - PACKAGE_TAIL_LEN;
#else  //WIN32
	nAppLen = m_nOutReportLen - PACKAGE_HEAD_LEN - PACKAGE_TAIL_LEN;
#endif //LINUX
	/*计算好每个小包的数据长度*/
	vecLen.assign(length / nAppLen, nAppLen);
	if ((nRemainLen = length % nAppLen) != 0) {
		vecLen.push_back(nRemainLen);
	}

	nOffset = 0;
	nSize = vecLen.size();
	
	/*获得当前会话id*/
	nSessionId = this->nextSessionId();
	for (i = 0; i < nSize; ++i) {		
		/*设置每个小包的数据*/
		appData.assign((char*)(data + nOffset), vecLen[i]);
		nOffset += vecLen[i];
		/*设置每个小包真实数据*/
		package.setAppData(appData);
		/*设置hid包长度*/
		package.setReportLen(m_nOutReportLen);
		/*设置会话id*/
		package.setSessionId(nSessionId);
		/*设置(当前发包次数/总次数) 类似:0x13 0x23 0x33*/
		package.setSendCount(((i + 1) << 4) | nSize);
		listRequest.push_back(package);
	}

	return 0;
}

int CHidChannel::sendAndRecv(unsigned char *data, size_t length, int nHopeSize, string &response, int nTimeout)
{
	int ret;
	list<CHidPackage> listRequest;
	list<CHidPackage>::iterator it;
	string packData;
	map<size_t, string>::iterator mit;
	unsigned char pRecv[1024] = {0};

	if (m_bOpened == false || data == NULL) {
		return -1;
	}

	/*将大包拆成多个小包*/
	if ((ret = this->makeRequest(data, length, listRequest)) < 0) {
		return -2;
	}

	/*将拆好的数据包连续写入plc设备*/
	for (it = listRequest.begin(); it != listRequest.end(); it++) {
		/*打包每个小包*/
		if ((ret = it->pack(packData)) < 0) {
			return -3;
		}	
	
		ret = m_hid.write(packData.c_str(), packData.size(), nTimeout);
		if (ret < 0) {
			return -5;
		}
	}
	/*等待读取响应包,可能大于64*/
	if (this->read(pRecv, m_nInputReportLen, response) <= 0) {
		return -6;
	}
	/*截取真实数据*/
	response.resize(nHopeSize);
	return response.size();
}

USBHID_END_NAMESPACE