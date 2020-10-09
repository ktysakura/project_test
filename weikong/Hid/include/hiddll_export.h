#ifndef HIDLL_EXPORT_H_20190911
#define HIDLL_EXPORT_H_20190911

#include <Windows.h>
#include "hid_namespace.h"

USBHID_BEGIN_NAMESPACE

typedef struct _HIDD_ATTRIBUTES {
	ULONG Size;
	USHORT VendorID;
	USHORT ProductID;
	USHORT VersionNumber;
} HIDD_ATTRIBUTES, *PHIDD_ATTRIBUTES;

//typedef USHORT USAGE;

typedef struct _HIDP_CAPS {
	USHORT Usage;
	USHORT UsagePage;
	USHORT InputReportByteLength;
	USHORT OutputReportByteLength;
	USHORT FeatureReportByteLength;
	USHORT Reserved[17];
	USHORT fields_not_used_by_hidapi[10];
} HIDP_CAPS, *PHIDP_CAPS;

typedef void *PHIDP_PREPARSED_DATA;

#define HIDP_STATUS_SUCCESS 0x110000

typedef BOOLEAN (__stdcall *HidD_GetAttributes_)(HANDLE device, PHIDD_ATTRIBUTES attrib);
typedef BOOLEAN (__stdcall *HidD_GetSerialNumberString_)(HANDLE device, PVOID buffer, ULONG buffer_len);
typedef BOOLEAN (__stdcall *HidD_GetManufacturerString_)(HANDLE handle, PVOID buffer, ULONG buffer_len);
typedef BOOLEAN (__stdcall *HidD_GetProductString_)(HANDLE handle, PVOID buffer, ULONG buffer_len);
typedef BOOLEAN (__stdcall *HidD_SetFeature_)(HANDLE handle, PVOID data, ULONG length);
typedef BOOLEAN (__stdcall *HidD_GetFeature_)(HANDLE handle, PVOID data, ULONG length);
typedef BOOLEAN (__stdcall *HidD_GetIndexedString_)(HANDLE handle, ULONG string_index, PVOID buffer, ULONG buffer_len);
typedef BOOLEAN (__stdcall *HidD_GetPreparsedData_)(HANDLE handle, PHIDP_PREPARSED_DATA *preparsed_data);
typedef BOOLEAN (__stdcall *HidD_FreePreparsedData_)(PHIDP_PREPARSED_DATA preparsed_data);
typedef BOOLEAN (__stdcall *HidP_GetCaps_)(PHIDP_PREPARSED_DATA preparsed_data, HIDP_CAPS *caps);
typedef BOOLEAN (__stdcall *HidD_SetNumInputBuffers_)(HANDLE handle, ULONG number_buffers);

//这些函数都是从hid.dll中导出的原型
typedef struct hiddll_functions_ {
	HidD_GetAttributes_ getAttributes;
	HidD_GetSerialNumberString_ getSerialNumberString;
	HidD_GetManufacturerString_ getManufactureString;
	HidD_GetProductString_ getProductString;
	HidD_SetFeature_ setFeature;
	HidD_GetFeature_ getFeature;
	HidD_GetIndexedString_ getIndexedString;
	HidD_GetPreparsedData_ getPreparsedData;
	HidD_FreePreparsedData_ freePreparseData;
	HidP_GetCaps_ getCaps;
	HidD_SetNumInputBuffers_ setNumInputBuffers;
} hiddll_functions;

int hiddll_getAllFunctions(HMODULE module, hiddll_functions *pAllFuncs);

USBHID_END_NAMESPACE


#endif