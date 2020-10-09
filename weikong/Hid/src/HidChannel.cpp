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
	/*��ȡ���Ļ�������С*/
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
	/*�ȳ��Զ�ȡ��һ����*/
	readLen = m_hid.read(pRecv, length, TIMEOUT_POLL_READ);
	if (readLen <= 0) {
		return readLen;
	}

	/*���ñ��ĳ���,*/
	package.setReportLen(length);
	/*���hid�Զ���Э���*/
	if (package.unpack(pRecv, readLen) < 0) {
		return -2;
	}

	nSessionID = package.getSessionId();
	nTotalCount = package.getTotalCount();//��ȡ�����Ҫ���ܴ���
	nCurCount = package.getCurrentCount();//��ȡ�����ǰ�Ĵ���
	sReponse.append(package.getAppData());
	/*����ǰ��Ҫ���յ��ܴ�������1,��������ʣ����Ӧ��*/
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
		/** session_id�����һ��,��ʾhidͨѶ����
			���һ�������ʾ����ͬһ�����
		*/
		if (nSessionID != package.getSessionId()) {
			return -4;
		}
		/*ƴ��������Ӧ���*/
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
	/*���������ʵ����:64 - 1 - 2 - 1 = 0,��һ���ֽڹ̶�Ϊ0x00*/
	nAppLen = m_nOutReportLen - 1 - PACKAGE_HEAD_LEN - PACKAGE_TAIL_LEN;
#else  //WIN32
	nAppLen = m_nOutReportLen - PACKAGE_HEAD_LEN - PACKAGE_TAIL_LEN;
#endif //LINUX
	/*�����ÿ��С�������ݳ���*/
	vecLen.assign(length / nAppLen, nAppLen);
	if ((nRemainLen = length % nAppLen) != 0) {
		vecLen.push_back(nRemainLen);
	}

	nOffset = 0;
	nSize = vecLen.size();
	
	/*��õ�ǰ�Ựid*/
	nSessionId = this->nextSessionId();
	for (i = 0; i < nSize; ++i) {		
		/*����ÿ��С��������*/
		appData.assign((char*)(data + nOffset), vecLen[i]);
		nOffset += vecLen[i];
		/*����ÿ��С����ʵ����*/
		package.setAppData(appData);
		/*����hid������*/
		package.setReportLen(m_nOutReportLen);
		/*���ûỰid*/
		package.setSessionId(nSessionId);
		/*����(��ǰ��������/�ܴ���) ����:0x13 0x23 0x33*/
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

	/*�������ɶ��С��*/
	if ((ret = this->makeRequest(data, length, listRequest)) < 0) {
		return -2;
	}

	/*����õ����ݰ�����д��plc�豸*/
	for (it = listRequest.begin(); it != listRequest.end(); it++) {
		/*���ÿ��С��*/
		if ((ret = it->pack(packData)) < 0) {
			return -3;
		}	
	
		ret = m_hid.write(packData.c_str(), packData.size(), nTimeout);
		if (ret < 0) {
			return -5;
		}
	}
	/*�ȴ���ȡ��Ӧ��,���ܴ���64*/
	if (this->read(pRecv, m_nInputReportLen, response) <= 0) {
		return -6;
	}
	/*��ȡ��ʵ����*/
	response.resize(nHopeSize);
	return response.size();
}

USBHID_END_NAMESPACE