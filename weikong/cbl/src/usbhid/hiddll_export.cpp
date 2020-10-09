#include <Windows.h>
#include "hiddll_export.h"

//从hid.pAllFuncs中导出函数
int hiddll_getAllFunctions(HMODULE module, hiddll_functions *pAllFuncs)
{
	if (module == NULL || pAllFuncs == NULL) {
		return -1;
	}

	if (module) {
		pAllFuncs->getAttributes = (HidD_GetAttributes_)GetProcAddress(module, "HidD_GetAttributes");
		if (!pAllFuncs->getAttributes) {
			return -2;
		}

		pAllFuncs->getSerialNumberString = (HidD_GetSerialNumberString_)GetProcAddress(module, "HidD_GetSerialNumberString");
		if (!pAllFuncs->getSerialNumberString) {
			return -3;
		}

		pAllFuncs->getManufactureString = (HidD_GetManufacturerString_)GetProcAddress(module, "HidD_GetManufacturerString");
		if (!pAllFuncs->getManufactureString) {
			return -4;
		}

		pAllFuncs->getProductString = (HidD_GetProductString_)GetProcAddress(module, "HidD_GetProductString");
		if (!pAllFuncs->getProductString) {
			return -5;
		}

		pAllFuncs->setFeature = (HidD_SetFeature_)GetProcAddress(module, "HidD_SetFeature");
		if (!pAllFuncs->setFeature) {
			return -6;
		}

		pAllFuncs->getFeature = (HidD_GetFeature_)GetProcAddress(module, "HidD_GetFeature");
		if (!pAllFuncs->getFeature) {
			return -7;
		}

		pAllFuncs->getIndexedString = (HidD_GetIndexedString_)GetProcAddress(module, "HidD_GetIndexedString");
		if (!pAllFuncs->getIndexedString) {
			return -8;
		}

		pAllFuncs->getPreparsedData = (HidD_GetPreparsedData_)GetProcAddress(module, "HidD_GetPreparsedData");
		if (!pAllFuncs->getPreparsedData) {
			return -9;
		}

		pAllFuncs->freePreparseData = (HidD_FreePreparsedData_)GetProcAddress(module, "HidD_FreePreparsedData");
		if (!pAllFuncs->freePreparseData) {
			return -10;
		}

		pAllFuncs->getCaps = (HidP_GetCaps_)GetProcAddress(module, "HidP_GetCaps");
		if (!pAllFuncs->getCaps) {
			return -11;
		}

		pAllFuncs->setNumInputBuffers = (HidD_SetNumInputBuffers_)GetProcAddress(module, "HidD_SetNumInputBuffers");
		if (!pAllFuncs->setNumInputBuffers) {
			return -12;
		}
	} else {
		return -13;
	}
	return 0;
}
