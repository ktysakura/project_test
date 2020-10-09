#ifndef CHID_H_20190911
#define CHID_H_20190911

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <setupapi.h>
#include <winioctl.h>
#include "hiddll_export.h"

namespace cbl {

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
	int open(unsigned short vid, unsigned short pid, const wchar_t *serial_number = NULL);

	//读函数，成功返回读的字节数，失败<0
	int read(void *data, size_t length);

	//写函数，成功返回写的字节数，失败返回<0
	int write(const void *data, size_t length);

	//关闭hid设备
	void close();

	bool isOpened();

	int setTimeout(int nReadTimeout, int nWriteTimeout);

	int get_feature_report(unsigned char *data, size_t length);
	int get_indexed_string(int str_index, wchar_t *str, size_t maxlen);
	int get_manufacturer_string(wchar_t *str, size_t maxlen);
	int get_product_string(wchar_t *str, size_t maxlen);
	int get_serial_number_string(wchar_t *str, size_t maxlen);
	int get_output_report_length();
	int get_input_report_length();

private:
	int init();
	int lookup();

	hid_device_info* enumerate(unsigned short vid, unsigned short pid);
	int free_enumerate(hid_device_info * devInfo);

	HANDLE open_device(const char *path, bool enumerateOpen);
	int open_path(const char *path);
	
	void new_hid_device();
	void free_hid_device();

private:
	bool m_inited;
	HMODULE m_module;
	hiddll_functions m_dllFuncs;
	hid_device m_device;
	int m_nReadTimeout;
	int m_nWriteTimeout;
};

}  //namespace cbl

#endif