#if !defined(BIG_ENDIAN_H__20180822)
#define BIG_ENDIAN_H__20180822

#include "DataType.h"

namespace cbl {
	
//����pBufferָ�뿪ʼ������2���ֽڰ����ģʽ��ʾ������ֵ���ɹ�����0��ʧ�ܷ��ظ���
int getBigendianValueU16(const void *pBuffer, U16 *result);

//��uValue�����ģʽд��pBufferָ�뿪ʼ������2���ֽ�λ�ã��ɹ�����0��ʧ�ܷ��ظ���
int setBigendianValueU16(U16 uValue, void *pBuffer);

//����pBufferָ�뿪ʼ������4���ֽڰ����ģʽ��ʾ������ֵ���ɹ�����0��ʧ�ܷ��ظ���
int getBigendianValueU32(const void *pBuffer, U32 *result);

//��uValue�����ģʽд��pBufferָ�뿪ʼ������4���ֽ�λ�ã��ɹ�����0��ʧ�ܷ��ظ���
int setBigendianValueU32(U32 uValue, void *pBuffer);

//����pBufferָ�뿪ʼ������8���ֽڰ����ģʽ��ʾ������ֵ���ɹ�����0��ʧ�ܷ��ظ���
int getBigendianValueU64(const void *pBuffer, U64 *result);

//��uValue�����ģʽд��pBufferָ�뿪ʼ������8���ֽ�λ�ã��ɹ�����0��ʧ�ܷ��ظ���
int setBigendianValueU64(U64 uValue, void *pBuffer);

} 

#endif
