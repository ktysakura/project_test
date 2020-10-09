#ifndef CHID_H_20190911
#define CHID_H_20190911
#include "hiddll_export.h"
#include "hid_namespace.h"
USBHID_BEGIN_NAMESPACE

typedef struct _hid_device {
	HANDLE device_handle;
	char *read_buf;
	OVERLAPPED ol_read;
	OVERLAPPED ol_write;
	size_t output_report_length;
	size_t input_report_length;
} hid_device;

typedef struct _hid_device_info {
	char *path;
	unsigned short vendor_id;
	unsigned short product_id;
	wchar_t *serial_number;
	unsigned short release_number;
	wchar_t *manufacturer_string;
	wchar_t *product_string;
	unsigned short usage_page;
	unsigned short usage;
	int interface_number;
	struct _hid_device_info *next;
} hid_device_info;
//usbhid设备读写类
class CHid {
public:
	CHid();
	virtual ~CHid();

	//根据vid/pid打开hid设备
	int open(unsigned short vid, unsigned short pid, const char* szPath, const wchar_t *serial_number = NULL);

	//带超时读函数，成功返回读的字节数，失败<0 length = 65
	int read(void *data, size_t length, size_t milliseconds);

	//带超时写函数，成功返回写的字节数，失败返回<0
	int write(const void *data, size_t length, size_t millsseconds);

	//清空缓冲区
	int flush();

	//关闭hid设备
	int close();

	int get_feature_report(unsigned char *data, size_t length);
	int get_indexed_string(int str_index, wchar_t *str, size_t maxlen);
	int get_manufacturer_string(wchar_t *str, size_t maxlen);
	int get_product_string(wchar_t *str, size_t maxlen);
	int get_serial_number_string(wchar_t *str, size_t maxlen);
	int get_output_report_length();
	int get_input_report_length();
	int init();
	hid_device_info* enumerate(unsigned short vid, unsigned short pid);
	int free_enumerate(hid_device_info * devInfo);
private:
	int lookup();
	HANDLE open_device(const char *path, bool enumerateOpen);
	int open_path(const char *path);
	
	void new_hid_device();
	void free_hid_device();

private:
	
	bool m_inited;
	HMODULE m_module;
	hiddll_functions m_dllFuncs;
	hid_device m_device;
};

USBHID_END_NAMESPACE

#endif