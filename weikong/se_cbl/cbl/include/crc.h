#ifndef CRC_H_20190917
#define CRC_H_20190917

#include <stdlib.h>

namespace cbl {

unsigned long crc32(const void *inBuf, size_t bytes);
int crc16(const void *inBuf, size_t bytes);
int cal_crc16(unsigned char *pData, int nSize);
unsigned long long int crc64(const void *inBuf, size_t bytes);


unsigned char xOR(const unsigned char *pbuf, int len);
}

#endif