#ifndef CPACKAGE_H_20190917
#define CPACKAGE_H_20190917
#include <iostream>
#include <string>
#include "hid_namespace.h"
using namespace std;

#define PACKAGE_HEAD_LEN		(1 + 1)	//��ͷ����
#define PACKAGE_TAIL_LEN		(1)					//��β����


USBHID_BEGIN_NAMESPACE

class CHidPackage {

public:
	//����sessionid
	void setSessionId(unsigned char sessionId);
	unsigned char getSessionId() const;
	//���õ�ǰ����/���ʹ�������(13,23,33)
	void setSendCount(unsigned char nSendCount);
	//��ȡ�����Ҫ���յ��ܴ���
	int getTotalCount() const;
	//��ȡ��ǰ���յĴ���
	int getCurrentCount() const;

	//���Ӧ�ò�����
	void setAppData(const string &appData);
	void setAppData(const char *appData, size_t length);
	const string& getAppData() const;

	//���������packDataΪ���ص�ͨѶ������
	int pack(string &packData);
	int pack(unsigned char *packData, int *dataLen);

	//�������
	int unpack(const string &unPackData);
	int unpack(const unsigned char *unPackData, int dataLen);

	//����usbhid�豸report��С��һ��д�������ȱ���Ϊ��ֵ
	void setReportLen(int len);
private:
	//���У��
	unsigned char calcul_Xor(const unsigned char *pbuf, int len);
private:
	unsigned char m_sessionId;
	unsigned char m_sendCount;
	string m_appData;
	int m_outReportLen;
};

USBHID_END_NAMESPACE

#endif