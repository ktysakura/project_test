#include "BigEndian.h"
#include <stdlib.h>

namespace cbl{

int getBigendianValueU16(const void *pBuffer, U16 *result)
{
	const unsigned char *ptr = (const unsigned char *)pBuffer;

	if (pBuffer == NULL || result == NULL) {
		return -1;
	}

	*result =  ((*ptr) << 8) | (*(ptr + 1));
	return 0;
}

int setBigendianValueU16(U16 uValue, void *pBuffer)
{
	unsigned char *ptr = (unsigned char *)pBuffer;

	if (pBuffer == NULL) {
		return -1;
	}

	*ptr 		= ((uValue >> 8) & 0xFF);
	*(ptr + 1) 	= (uValue & 0xFF);

	return 0;
}

int getBigendianValueU32(const void *pBuffer, U32 *result)
{
	const unsigned char *ptr = (const unsigned char *)pBuffer;

	if (pBuffer == NULL || result == NULL) {
		return -1;
	}

	*result = ((*ptr) << 24) |
			  ((*(ptr + 1)) << 16) |
			  ((*(ptr + 2)) << 8) |
			  (*(ptr + 3));

	return 0;	
}

int setBigendianValueU32(U32 uValue, void *pBuffer)
{
	unsigned char *ptr = (unsigned char *)pBuffer;

	if (pBuffer == NULL) {
		return -1;
	}

	*ptr 		= ((uValue >> 24) & 0xFF);
	*(ptr + 1) 	= ((uValue >> 16) & 0xFF);
	*(ptr + 2) 	= ((uValue >> 8) & 0xFF);
	*(ptr + 3) 	= (uValue & 0xFF);
		
	return 0;
}

int getBigendianValueU64(const void *pBuffer, U64 *result)
{
	const unsigned char *ptr = (const unsigned char *)pBuffer;
	U64 uValue = 0;

	if (pBuffer == NULL || result == NULL) {
		return -1;
	}

	for (int i = 0; i < 8; i++) {
		uValue <<= 8;
		uValue |= *ptr++;
	}

	*result = uValue;
	return 0;	
}

int setBigendianValueU64(U64 uValue, void *pBuffer)
{
	unsigned char *ptr = (unsigned char *)pBuffer;

	if (pBuffer == NULL) {
		return -1;
	}

	for (int i = 0; i < 8; i++) {
		*ptr++ = ((uValue >> ((7 - i) << 3)) & 0xFF);
	}

	return 0;	
}
	
}
