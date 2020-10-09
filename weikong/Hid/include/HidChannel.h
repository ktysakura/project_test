#ifndef CHIDCHANNEL_H_20190916
#define CHIDCHANNEL_H_20190916

#include <string>
#include <list>
#include <vector>

#ifdef WIN32
#include "Hid.h"
#else
#include <string.h>
#include <pthread.h> 
#include "Hid.h"
#endif

#include "hid_namespace.h"
#include "HidPackage.h"
#include "SemaphoreEx.h"
#include "SemaphorePool.h"

using namespace cbl;
using namespace std;

#define TIMEOUT_FRAME_WRITE				1000	//send函数中单个帧的写超时时间
#define TIMEOUT_FRAME_WAIT				3000	//send函数中单个帧等待应答的信号灯超时时间
#define TIMEOUT_REPLY_WRITE				1000	//reply函数中的写超时时间
#define TIMEOUT_POLL_READ				500   	//线程轮询读的超时时间
#define TIMEOUT_WAIT_THREAD_EXIT		4000	//等待读线程退出的超时时间



USBHID_BEGIN_NAMESPACE

class CHidChannel;

//用于接收请求包的回调函数
//typedef int (*pf_onRecvRequest)(CHidChannel *pHidChannel, CPackage &package);

//用于接收数据传输的回调函数
//typedef int (*pf_onRecvData)(CHidChannel *pHidChannel, CPackage &package);

//usbhid通道类
class  CHidChannel
{
public:
	CHidChannel(unsigned char initSessionId = 0);
	virtual ~CHidChannel();
#ifdef WIN32
	//打开hid设备
	int open(unsigned short vid, unsigned short pid, const char* szPath, const wchar_t *serial_number = NULL);
#else
	int open(const char *hidName, int mode);
#endif
	bool isOpen();
	//发送请求函数，通过response接收应答内容
	int sendAndRecv(unsigned char *data, size_t length, int nHopeSize, string &response, int nTimeout);
	//停止usbhid通道，接收线程结束
	int close();
private:
	//获取会话id
	unsigned char nextSessionId(int nCount = 1);
	int read(unsigned char *pRecv, size_t length, string &sReponse);
	int makeRequest(unsigned char *data, size_t length, list<CHidPackage> &listRequest);
private:
	bool m_bOpened;
	unsigned char m_sessionId;
	int m_nOutReportLen;
	int m_nInputReportLen;
	bool m_bStop;
	CHid m_hid;
};


USBHID_END_NAMESPACE




#endif