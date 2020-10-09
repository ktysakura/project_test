#ifndef CPACKAGE_H_20190917
#define CPACKAGE_H_20190917
#include <iostream>
#include <string>
#include "hid_namespace.h"
using namespace std;

#define PACKAGE_HEAD_LEN		(1 + 1)	//包头长度
#define PACKAGE_TAIL_LEN		(1)					//包尾长度


USBHID_BEGIN_NAMESPACE

class CHidPackage {

public:
	//设置sessionid
	void setSessionId(unsigned char sessionId);
	unsigned char getSessionId() const;
	//设置当前次数/发送次数，若(13,23,33)
	void setSendCount(unsigned char nSendCount);
	//获取大包需要接收的总次数
	int getTotalCount() const;
	//获取当前接收的次数
	int getCurrentCount() const;

	//填充应用层数据
	void setAppData(const string &appData);
	void setAppData(const char *appData, size_t length);
	const string& getAppData() const;

	//打包函数，packData为返回的通讯包数据
	int pack(string &packData);
	int pack(unsigned char *packData, int *dataLen);

	//解包函数
	int unpack(const string &unPackData);
	int unpack(const unsigned char *unPackData, int dataLen);

	//设置usbhid设备report大小，一次写操作长度必须为该值
	void setReportLen(int len);
private:
	//异或校验
	unsigned char calcul_Xor(const unsigned char *pbuf, int len);
private:
	unsigned char m_sessionId;
	unsigned char m_sendCount;
	string m_appData;
	int m_outReportLen;
};

USBHID_END_NAMESPACE

#endif