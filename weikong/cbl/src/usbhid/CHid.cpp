#include "CHid.h"
#include "../base/DataType.h"

#pragma comment(lib, "setupapi.lib")

#define MAX_STRING_WCHARS	0xFFF

#define HID_OUT_CTL_CODE(id)   CTL_CODE(FILE_DEVICE_KEYBOARD, (id), METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_HID_GET_FEATURE  HID_OUT_CTL_CODE(100)

#define DEFAULT_READ_TIMEOUT 	5000
#define DEFAULT_WRITE_TIMEOUT 	5000

namespace cbl {

CHid::CHid()
{
	m_inited = false;
	m_module = NULL;
	memset(&m_device, 0, sizeof(m_device));
	memset(&m_dllFuncs, 0, sizeof(hiddll_functions));
	m_nReadTimeout 	= DEFAULT_READ_TIMEOUT;
	m_nWriteTimeout = DEFAULT_WRITE_TIMEOUT;
}

CHid::~CHid()
{
	m_inited = false;
	if (m_module) {
		FreeLibrary(m_module);
		m_module = NULL;
	}
}

int CHid::init(void)
{
	if (!m_inited) {
		if (lookup() < 0) {
			return -1;
		}
		m_inited = true;
	}
	return 0;
}

int CHid::lookup()
{
	m_module = LoadLibraryA("hid.dll");
	return hiddll_getAllFunctions(m_module, &m_dllFuncs);
}

hid_device_info* CHid::enumerate(unsigned short vid, unsigned short pid) 
{
    BOOL res;
    hid_device_info *root = NULL;
    hid_device_info *cur_dev = NULL;

    GUID InterfaceClassGuid = {0x4d1e55b2, 0xf16f, 0x11cf, {0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30}};
    SP_DEVINFO_DATA devinfo_data;
    SP_DEVICE_INTERFACE_DATA device_interface_data;
    SP_DEVICE_INTERFACE_DETAIL_DATA_A *device_interface_detail_data = NULL;
    HDEVINFO device_info_set = INVALID_HANDLE_VALUE;
    int device_index = 0;
    int i;

    if (init() < 0) {
        return -NULL;
    }

    memset(&devinfo_data, 0x0, sizeof(devinfo_data));
    devinfo_data.cbSize = sizeof(SP_DEVINFO_DATA);
    device_interface_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    device_info_set = SetupDiGetClassDevsA(&InterfaceClassGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    for (;;) {
        HANDLE write_handle = INVALID_HANDLE_VALUE;
        DWORD required_size = 0;
        HIDD_ATTRIBUTES attrib;

        res = SetupDiEnumDeviceInterfaces(device_info_set,
                                          NULL,
                                          &InterfaceClassGuid,
                                          device_index,
                                          &device_interface_data);

        if (!res) {
            break;
        }

        res = SetupDiGetDeviceInterfaceDetailA(device_info_set,
                                               &device_interface_data,
                                               NULL,
                                               0,
                                               &required_size,
                                               NULL);

        device_interface_detail_data = (SP_DEVICE_INTERFACE_DETAIL_DATA_A *)malloc(required_size);
        device_interface_detail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);

        res = SetupDiGetDeviceInterfaceDetailA(device_info_set,
                                               &device_interface_data,
                                               device_interface_detail_data,
                                               required_size,
                                               NULL,
                                               NULL);
        if (!res) {
            goto cont;
        }

        for (i = 0; ; i++) {
            char driver_name[256];

            res = SetupDiEnumDeviceInfo(device_info_set, i, &devinfo_data);
            if (!res) {
                goto cont;
            }

            res = SetupDiGetDeviceRegistryPropertyA(device_info_set, &devinfo_data,
                                                    SPDRP_CLASS, NULL, (PBYTE)driver_name, sizeof(driver_name), NULL);
            if (!res) {
                goto cont;
            }

            if (strcmp(driver_name, "HIDClass") == 0) {
                res = SetupDiGetDeviceRegistryPropertyA(device_info_set, &devinfo_data,
                                                        SPDRP_DRIVER, NULL, (PBYTE)driver_name, sizeof(driver_name), NULL);
                if (res) {
                    break;
                }
            }
        }

        write_handle = open_device(device_interface_detail_data->DevicePath, TRUE);
        if (write_handle == INVALID_HANDLE_VALUE) {
            goto cont_close;
        }

        attrib.Size = sizeof(HIDD_ATTRIBUTES);
        m_dllFuncs.getAttributes(write_handle, &attrib);

        if ((vid == 0x0 || attrib.VendorID == vid) &&
            (pid == 0x0 || attrib.ProductID == pid)) {
			const int WSTR_LEN = 512;
            const char *str;
            hid_device_info *tmp;
            PHIDP_PREPARSED_DATA pp_data = NULL;
            HIDP_CAPS caps;
            BOOLEAN res;
            NTSTATUS nt_res;
            wchar_t wstr[WSTR_LEN];
            size_t len;

            tmp = (hid_device_info *)calloc(1, sizeof(hid_device_info));
            if (cur_dev) {
                cur_dev->next = tmp;
            }
            else {
                root = tmp;
            }
            cur_dev = tmp;

            res = m_dllFuncs.getPreparsedData(write_handle, &pp_data);
            if (res) {
                nt_res = m_dllFuncs.getCaps(pp_data, &caps);

                if (nt_res == HIDP_STATUS_SUCCESS) {
                    cur_dev->usage_page = caps.UsagePage;
                    cur_dev->usage = caps.Usage;
                }

				m_dllFuncs.freePreparseData(pp_data);
            }

            cur_dev->next = NULL;
            str = device_interface_detail_data->DevicePath;

            if (str) {
                len = strlen(str);
                cur_dev->path = (char *) calloc(len + 1, sizeof(char));
                strncpy(cur_dev->path, str, len + 1);
                cur_dev->path[len] = '\0';
            } else {
                cur_dev->path = NULL;
            }

            res = m_dllFuncs.getSerialNumberString(write_handle, wstr, sizeof(wstr));
            wstr[WSTR_LEN - 1] = 0x0000;
            if (res) {
                cur_dev->serial_number = _wcsdup(wstr);
            }

            res = m_dllFuncs.getManufactureString(write_handle, wstr, sizeof(wstr));
            wstr[WSTR_LEN - 1] = 0x0000;
            if (res) {
                cur_dev->manufacturer_string = _wcsdup(wstr);
            }

            res = m_dllFuncs.getProductString(write_handle, wstr, sizeof(wstr));
            wstr[WSTR_LEN - 1] = 0x0000;
            if (res) {
                cur_dev->product_string = _wcsdup(wstr);
            }

            cur_dev->vendor_id = attrib.VendorID;
            cur_dev->product_id = attrib.ProductID;

            cur_dev->release_number = attrib.VersionNumber;
            cur_dev->interface_number = -1;

            if (cur_dev->path) {
                char *interface_component = strstr(cur_dev->path, "&mi_");

                if (interface_component) {
                    char *hex_str = interface_component + 4;
                    char *endptr = NULL;

                    cur_dev->interface_number = strtol(hex_str, &endptr, 16);
                    if (endptr == hex_str) {
                        cur_dev->interface_number = -1;
                    }
                }
            }
        }

cont_close:
        CloseHandle(write_handle);
cont:
        free(device_interface_detail_data);
        device_index++;
    }

    SetupDiDestroyDeviceInfoList(device_info_set);
    return root;
}

int CHid::free_enumerate(hid_device_info *devInfo)
{
	hid_device_info *d = devInfo;

	while (d) {
		hid_device_info *next = d->next;
		free(d->path);
		free(d->serial_number);
		free(d->manufacturer_string);
		free(d->product_string);
		free(d);
		d = next;
	}
	return 0;
}

HANDLE CHid::open_device(const char *path, bool enumerateOpen)
{
	HANDLE handle;
	DWORD desired_access = (enumerateOpen) ? 0 : (GENERIC_WRITE | GENERIC_READ);
	DWORD share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;

	handle = CreateFileA(path,
				desired_access,
				share_mode,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_OVERLAPPED,
				0);

	return handle;
}

int CHid::open(unsigned short vid, unsigned short pid, const wchar_t *serial_number)
{
	hid_device_info *devs, *cur_dev;
	const char *path_to_open = NULL;
	int retCode = 0;

	if (!m_inited) {
		if (init() < 0) {
			return -1;
		}
	}

	devs = enumerate(vid, pid);
	cur_dev = devs;

	while (cur_dev) {
		if (cur_dev->vendor_id == vid && cur_dev->product_id == pid) {
				if (serial_number) {
					if (wcscmp(serial_number, cur_dev->serial_number) == 0) {
						path_to_open = cur_dev->path;
						break;
					}
				} else {
					path_to_open = cur_dev->path;
					break;
				}
		}
		cur_dev = cur_dev->next;
	}

	if (path_to_open) {
		if (open_path(path_to_open) < 0) {
			retCode = -2;
			goto exit;
		}
	} else {
		retCode = -3;
		goto exit;
	}

exit:
	free_enumerate(devs);
	return retCode;
}

void CHid::close()
{
	CancelIo(m_device.device_handle);
	free_hid_device();
}

int CHid::open_path(const char *path)
{
	hid_device *dev;
	HIDP_CAPS caps;
	PHIDP_PREPARSED_DATA pp_data = NULL;
	BOOLEAN res;
	NTSTATUS nt_res;
	int retCode = 0;

	if (init() < 0) {
		return -1;
	}

	new_hid_device();
	dev = &m_device;

	dev->device_handle = open_device(path, false);
	if (dev->device_handle == INVALID_HANDLE_VALUE) {
		retCode = -2;
		goto err;
	}

	res = m_dllFuncs.setNumInputBuffers(dev->device_handle, 64);
	if (!res) {
		retCode = -3;
		goto err;
	}

	res = m_dllFuncs.getPreparsedData(dev->device_handle, &pp_data);
	if (!res) {
		retCode = -4;
		goto err;
	}

	nt_res = m_dllFuncs.getCaps(pp_data, &caps);
	if (nt_res != HIDP_STATUS_SUCCESS) {
		retCode = -5;
		goto err_pp_data;
	}

	dev->output_report_length = caps.OutputReportByteLength;
	dev->input_report_length = caps.InputReportByteLength;
	m_dllFuncs.freePreparseData(pp_data);

	dev->read_buf = (char *)malloc(dev->input_report_length);
	memset(dev->read_buf, 0, dev->input_report_length);
	return 0;

err_pp_data:
	m_dllFuncs.freePreparseData(pp_data);

err:
	free_hid_device();
	return retCode;
}

void CHid::new_hid_device()
{
	hid_device *dev = &m_device;

	dev->device_handle = INVALID_HANDLE_VALUE;
	dev->output_report_length = 0;
	dev->input_report_length = 0;
	dev->read_buf = NULL;
	memset(&dev->ol_read, 0, sizeof(dev->ol_read));
	memset(&dev->ol_write, 0, sizeof(dev->ol_write));
	dev->ol_read.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	dev->ol_write.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

void CHid::free_hid_device()
{
	hid_device *dev = &m_device;

	CloseHandle(dev->ol_read.hEvent);
	CloseHandle(dev->ol_write.hEvent);
	CloseHandle(dev->device_handle);

	if (dev->read_buf) {
		free(dev->read_buf);
		dev->read_buf = NULL;
	}
}

int CHid::get_feature_report(unsigned char *data, size_t length)
{
    BOOL res;
    DWORD bytes_returned;
    OVERLAPPED ol;

    memset(&ol, 0, sizeof(ol));

	if (data == NULL) {
		return -1;
	}

	if (m_device.device_handle == INVALID_HANDLE_VALUE) {
		return -2;
	}

    res = DeviceIoControl(m_device.device_handle,
                          IOCTL_HID_GET_FEATURE,
                          data, length,
                          data, length,
                          &bytes_returned, &ol);
    if (!res) {
        if (GetLastError() != ERROR_IO_PENDING) {
			return -3;
        }
    }

    res = GetOverlappedResult(m_device.device_handle, &ol, &bytes_returned, TRUE);
    if (!res) {
        return -4;
    }

    bytes_returned++;
    return bytes_returned;
}

int CHid::get_indexed_string(int str_index, wchar_t *str, size_t maxlen)
{
	BOOL res;

	if (str == NULL) {
		return -1;
	}

	if (m_device.device_handle == INVALID_HANDLE_VALUE) {
		return -2;
	}

	res = m_dllFuncs.getIndexedString(m_device.device_handle, str_index, str, sizeof(wchar_t) * cbl_min(maxlen, MAX_STRING_WCHARS));
	if (!res) {
		return -3;
	}

	return 0;
}

int CHid::get_manufacturer_string(wchar_t *str, size_t maxlen)
{
	BOOL res;

	if (str == NULL) {
		return -1;
	}

	if (m_device.device_handle == INVALID_HANDLE_VALUE) {
		return -2;
	}

	res = m_dllFuncs.getManufactureString(m_device.device_handle, str, sizeof(wchar_t) * cbl_min(maxlen, MAX_STRING_WCHARS));
	if (!res) {
		return -3;
	}

	return 0;
}

int CHid::get_product_string(wchar_t *str, size_t maxlen)
{
	BOOL res;

	if (str == NULL) {
		return -1;
	}

	if (m_device.device_handle == INVALID_HANDLE_VALUE) {
		return -2;
	}

	res = m_dllFuncs.getProductString(m_device.device_handle, str, sizeof(wchar_t) * cbl_min(maxlen, MAX_STRING_WCHARS));
	if (!res) {
		return -3;
	}

	return 0;
}

int CHid::get_serial_number_string(wchar_t *str, size_t maxlen)
{
	BOOL res;

	if (str == NULL) {
		return -1;
	}

	if (m_device.device_handle == INVALID_HANDLE_VALUE) {
		return -2;
	}

	res = m_dllFuncs.getSerialNumberString(m_device.device_handle, str, sizeof(wchar_t) * cbl_min(maxlen, MAX_STRING_WCHARS));
	if (!res) {
		return -3;
	}

	return 0;
}

int CHid::write(const void *data, size_t length)
{
	BOOL res;
	DWORD bytes_write = 0;
	hid_device *dev = &m_device;
	HANDLE ev = dev->ol_write.hEvent;

	if (data == NULL) {
		return -1;
	}

	if (m_device.device_handle == INVALID_HANDLE_VALUE) {
		return -2;
	}

	if (length < m_device.output_report_length) {
		return -3;
	}

	ResetEvent(ev);
    WriteFile(dev->device_handle, data, length, NULL, &dev->ol_write);
	res = WaitForSingleObject(ev, m_nWriteTimeout);
	if (res == WAIT_TIMEOUT) {
		return -3;
	}

	res = GetOverlappedResult(dev->device_handle, &dev->ol_write, &bytes_write, FALSE);
	if (!res) {
		return -4;
	}

    return bytes_write;
}

int CHid::read(void *data, size_t length)
{
    DWORD bytes_read = 0;
    size_t copy_len = 0;
    BOOL res;
	hid_device *dev = &m_device;
    HANDLE ev = dev->ol_read.hEvent;

	if (data == NULL || m_inited == false) {
		return -1;
	}

	if (m_device.device_handle == INVALID_HANDLE_VALUE) {
		return -2;
	}

	memset(dev->read_buf, 0, dev->input_report_length);
	ResetEvent(ev);
	ReadFile(dev->device_handle, dev->read_buf, dev->input_report_length, &bytes_read, &dev->ol_read);

	res = WaitForSingleObject(ev, m_nReadTimeout);
	if (res == WAIT_TIMEOUT) {
		return -3;
	}

	res = GetOverlappedResult(dev->device_handle, &dev->ol_read, &bytes_read, FALSE);
	if (res && bytes_read > 0) {
		copy_len = length > bytes_read ? bytes_read : length;
		memcpy(data, dev->read_buf, copy_len);
		return copy_len;
	} else {
		return -4;
	}

    return 0;
}

int CHid::get_output_report_length()
{
	if (m_device.device_handle == INVALID_HANDLE_VALUE) {
		return -1;
	}

	return m_device.output_report_length;
}

int CHid::get_input_report_length()
{
	if (m_device.device_handle == INVALID_HANDLE_VALUE) {
		return -1;
	}

	return m_device.input_report_length;
}

bool CHid::isOpened()
{
	return (m_device.device_handle != INVALID_HANDLE_VALUE);
}

int CHid::setTimeout(int nReadTimeout, int nWriteTimeout)
{
	m_nReadTimeout = nReadTimeout;
	m_nWriteTimeout = nWriteTimeout;
	return 0;
}

} //namespace cbl
