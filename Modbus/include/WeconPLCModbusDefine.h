#if !defined(_MODBUS_WECON_PLC_DEFINE_H__20190922_)
#define _MODBUS_WECON_PLC_DEFINE_H__20190922_

#define WECON_PLC_ENCRYPTION_VERSION (0x64)

namespace cbl{

	/*�û��Զ��幦����*/
	typedef enum{
		WECON_PLC_FUNCTION_CODE_READ= 0x41,               /*Wecon PLC������*/
		WECON_PLC_FUNCTION_CODE_WRITE = 0x42,             /*Wecon PLCд����*/
		WECON_PLC_FUNCTION_CODE_WRITE_SUB_CODE = 0x43     /*PLC������*/
	}WECON_PLC_FUNCTION_CODE_T;
	

	typedef enum{
		UPLOAD_KEY,
		DOWNLOAD_KEY,
		CLOCK_KEY,
		PLCID_KEY,
		BANUPLOAD_KEY,
		END_KEY
}PASSWORD_KEY_TYPE;

	/*����*/
	typedef enum{
		FUNCTION_SUB_CODE_DEL_LD = 0x00,                   /*ɾ������ͼ*/
		FUNCTION_SUB_CODE_RW_INTERIOR = 0x01,	           /*��д�ڲ���ַ*/
		FUNCTION_SUB_CODE_RESTORE_PLC_PARAM = 0x01,        /*��ԭPLC��������*/
		FUNCTION_SUB_CODE_CLEAR_BIT_DEV = 0x02,            /*���λ��Ԫ��*/
		FUNCTION_SUB_CODE_CLEAR_WORD_DEV = 0x03,           /*�������Ԫ��*/
		FUNCTION_SUB_CODE_CLEAR_DATA_DEV = 0x04,           /*���������Ԫ��*/
		FUNCTION_SUB_CODE_RESTORE_SPECIAL_DEV_PARAM = 0x05,/*��ԭ������Ԫ��*/
		FUNCTION_SUB_CODE_CLEAR_PLC_ERROR = 0x06,          /*���PLC����*/
		FUNCTION_SUB_CODE_PREPARE_DOWNLOAD_LD = 0x10,      /*׼����������ͼ*/
		FUNCTION_SUB_CODE_READ_MODULE = 0x10,              /*��ȡģ������*/
		FUNCTION_SUB_CODE_WRITE_MODULE =0x10,              /*д��ģ������*/
		FUNCTION_SUB_CODE_IS_DOWNLOAD_LD =  0x11,          /*ѯ��PLC�ܷ���������ͼ*/
		FUNCTION_SUB_CODE_PREPARE_DOWNLOAD_COMMENT = 0x12, /*׼������ע��*/
		FUNCTION_SUB_CODE_IS_DOWNLOAD_COMMENT = 0x13,      /*ѯ��PLC�ܷ�����ע��*/
		FUNCTION_SUB_CODE_PREPARE_DOWNLOAD_PARAMETER = 0x14,/*׼�����ز���*/
		FUNCTION_SUB_CODE_FINISH_DOWNLOAD = 0x1E,          /*�������*/
		FUNCTION_SUB_CODE_DOWNLOAD_INT = 0x1F,             /*�����ж�*/
		FUNCTION_SUB_CODE_WRITE_BITINREGISTER = 0x20,      /*����дλ*/
		FUNCTION_SUB_CODE_WRITE_PASSWORD = 0x60,           /*д������*/
		FUNCTION_SUB_CODE_TEMPORARY_DECRYPT = 0x61,        /*��ʱ����*/
		FUNCTION_SUB_CODE_VERIFY_PASSWORD = 0x62,          /*�����д��֤*/
		FUNCTION_SUB_CODE_REMOVE_PASSWORD = 0x63,          /*ɾ������*/
		FUNCTION_SUB_CODE_REMOVE_TEMPORARY_DECRYPT = 0x64, /*�����ʱ����*/
		FUNCTION_SUB_CODE_READ_ENCRYPT_MESSAGE  = 0x65,    /*��ȡ������Ϣ*/
		FUNCTION_SUB_CODE_READ_RANDOM_SEED = 0x66,         /*��ȡ�������*/
		FUNCTION_SUB_CODE_CHECK_PASSWORD = 0x67			   /*������֤*/
	}WENCON_PLC_FUNCTION_SUB_CODE_T;
	
	

}
#endif