#include "des.h"
#include <stdlib.h>

namespace cbl {

#define LB32_MASK 0x00000001
#define LB64_MASK 0x0000000000000001
#define L64_MASK  0x00000000ffffffff

static const char PC1[] =
{
	57, 49, 41, 33, 25, 17,  9,
	1, 58, 50, 42, 34, 26, 18,
	10,  2, 59, 51, 43, 35, 27,
	19, 11,  3, 60, 52, 44, 36,

	63, 55, 47, 39, 31, 23, 15,
	7, 62, 54, 46, 38, 30, 22,
	14,  6, 61, 53, 45, 37, 29,
	21, 13,  5, 28, 20, 12,  4
};

static const char PC2[] =
{
	14, 17, 11, 24,  1,  5,
	3, 28, 15,  6, 21, 10,
	23, 19, 12,  4, 26,  8,
	16,  7, 27, 20, 13,  2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32
};

static const char ITERATION_SHIFT[] =
{
	1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

static const char IP[] =
{
	58, 50, 42, 34, 26, 18, 10, 2,
	60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6,
	64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17,  9, 1,
	59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5,
	63, 55, 47, 39, 31, 23, 15, 7
};

static const char FP[] =
{
	40, 8, 48, 16, 56, 24, 64, 32,
	39, 7, 47, 15, 55, 23, 63, 31,
	38, 6, 46, 14, 54, 22, 62, 30,
	37, 5, 45, 13, 53, 21, 61, 29,
	36, 4, 44, 12, 52, 20, 60, 28,
	35, 3, 43, 11, 51, 19, 59, 27,
	34, 2, 42, 10, 50, 18, 58, 26,
	33, 1, 41,  9, 49, 17, 57, 25
};

static const char EXPANSION[] =
{
	32,  1,  2,  3,  4,  5,
	4,  5,  6,  7,  8,  9,
	8,  9, 10, 11, 12, 13,
	12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21,
	20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29,
	28, 29, 30, 31, 32,  1
};


static const char SBOX[8][64] =
{
	{
		// S1
		14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
		0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
		4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
		15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13
	},
	{
		// S2
		15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
		3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
		0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
		13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9
	},
	{
		// S3
		10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
		13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
		13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
		1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12
	},
	{
		// S4
		7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
		13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
		10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
		3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14
	},
	{
		// S5
		2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
		14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
		4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
		11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3
	},
	{
		// S6
		12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
		10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
		9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
		4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13
	},
	{
		// S7
		4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
		13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
		1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
		6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12
	},
	{
		// S8
		13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
		1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
		7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
		2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
	}
};

static const char PBOX[] =
{
	16,  7, 20, 21,
	29, 12, 28, 17,
	1, 15, 23, 26,
	5, 18, 31, 10,
	2,  8, 24, 14,
	32, 27,  3,  9,
	19, 13, 30,  6,
	22, 11,  4, 25
};

void stdDes::keygen(ui64 key)
{
    ui64 permuted_choice_1 = 0;

    for (ui8 i = 0; i < 56; i++) {
        permuted_choice_1 <<= 1;
        permuted_choice_1 |= (key >> (64-PC1[i])) & LB64_MASK;
    }

    ui32 C = (ui32) ((permuted_choice_1 >> 28) & 0x000000000fffffff);
    ui32 D = (ui32)  (permuted_choice_1 & 0x000000000fffffff);

    for (ui8 i = 0; i < 16; i++) {
        for (ui8 j = 0; j < ITERATION_SHIFT[i]; j++) {
            C = (0x0fffffff & (C << 1)) | (0x00000001 & (C >> 27));
            D = (0x0fffffff & (D << 1)) | (0x00000001 & (D >> 27));
        }

        ui64 permuted_choice_2 = (((ui64) C) << 28) | (ui64) D;

        sub_key[i] = 0;
        for (ui8 j = 0; j < 48; j++) {
            sub_key[i] <<= 1;
            sub_key[i] |= (permuted_choice_2 >> (56-PC2[j])) & LB64_MASK;
        }
    }
}

ui64 stdDes::ui64Des(ui64 block, bool mode)
{
    block = ip(block);

    ui32 L = (ui32) (block >> 32) & L64_MASK;
    ui32 R = (ui32) (block & L64_MASK);

    for (ui8 i = 0; i < 16; i++) {
        ui32 F = mode ? f(R, sub_key[15-i]) : f(R, sub_key[i]);
        feistel(L, R, F);
    }

    block = (((ui64) R) << 32) | (ui64) L;

    return fp(block);
}

ui64 stdDes::ip(ui64 block)
{
    ui64 result = 0;

    for (ui8 i = 0; i < 64; i++) {
        result <<= 1;
        result |= (block >> (64-IP[i])) & LB64_MASK;
    }
    return result;
}

ui64 stdDes::fp(ui64 block)
{
    ui64 result = 0;

    for (ui8 i = 0; i < 64; i++) {
        result <<= 1;
        result |= (block >> (64-FP[i])) & LB64_MASK;
    }
    return result;
}

void stdDes::feistel(ui32 &L, ui32 &R, ui32 F)
{
    ui32 temp = R;
    R = L ^ F;
    L = temp;
}

ui32 stdDes::f(ui32 R, ui64 k)
{
    ui64 s_input = 0;

    for (ui8 i = 0; i < 48; i++) {
        s_input <<= 1;
        s_input |= (ui64) ((R >> (32-EXPANSION[i])) & LB32_MASK);
    }

    s_input = s_input ^ k;

    ui32 s_output = 0;
    for (ui8 i = 0; i < 8; i++) {
        char row = (char) ((s_input & (0x0000840000000000 >> 6*i)) >> (42-6*i));
        row = (row >> 4) | (row & 0x01);

        char column = (char) ((s_input & (0x0000780000000000 >> 6*i)) >> (43-6*i));

        s_output <<= 4;
        s_output |= (ui32) (SBOX[i][16*row + column] & 0x0f);
    }

    ui32 f_result = 0;
    for (ui8 i = 0; i < 32; i++)
    {
        f_result <<= 1;
        f_result |= (s_output >> (32 - PBOX[i])) & LB32_MASK;
    }

    return f_result;
}


//把16个16进制字符以大端序转储为ui64
//"1234567890abcdef"转为0x1234567890abcdef
ui64 stdDes::strToBigEndianUi64(const char *str)
{
	ui64 ret = 0;
	unsigned char *pRet = (unsigned char *)&ret;

	if (str == NULL || strlen(str) < 16) {
		return 0;
	}

	for (int i = 0; i < 8; i++) {
		char temp[3] = {0};

		strncpy(temp, &str[2 * i], 2);
		pRet[7-i] = (unsigned char)strtol(temp, NULL, 16);
	}

	return ret;
}

//把8个字节数组以大端序转储为ui64
//"\x12\x34\x56\x78\x90\xab\xcd\xef"转为0x1234567890abcdef
ui64 stdDes::bytesToBigEndianUi64(const unsigned char *bytes, int bytesLen)
{
	ui64 ret = 0;
	unsigned char *pRet = (unsigned char *)&ret;

	if (bytes == NULL || bytesLen < 8) {
		return 0;
	}

	for (int i = 0; i < 8; i++) {
		pRet[7-i] = bytes[i];
	}

	return ret;
}

//ui64以大端序转储为字节数组
//0x1234567890abcdef转为"\x12\x34\x56\x78\x90\xab\xcd\xef"
int stdDes::ui64ToBigEndianBytes(ui64 value, unsigned char *out, int outLen)
{
	unsigned char *pRet = (unsigned char *)&value;

	if (out == NULL || outLen < 8) {
		return -1;
	}

	for (int i = 0; i < 8; i++) {
		out[i] = pRet[7-i];
	}

	return 0;
}

void stdDes::setKey(ui64 key)
{
	keygen(key);
	isKeySet = true;
}

int stdDes::setKey(const unsigned char *key, int keyLen)
{
	ui64 k;

	if (key == NULL || keyLen < 8) {
		return -1;
	}

	k = bytesToBigEndianUi64(key, keyLen);
	setKey(k);
	return 0;
}

void stdDes::setDesMode(E_DesMode mode, ui64 initVector)
{
	desMode = mode;
	iv = initVector;
	lastBlock = iv;
}

int stdDes::setDesMode(E_DesMode mode, const unsigned char *initVector, int vecLen)
{
	if (initVector == NULL || vecLen < 8) {
		return -1;
	}

	desMode = mode;
	iv = stdDes::bytesToBigEndianUi64(initVector, 8);
	lastBlock = iv;

	return 0;
}

void stdDes::resetIv()
{
	lastBlock = iv;
}

int stdDes::ui64Encrypt(ui64 block, ui64 *out)
{
	if (out == NULL) {
		return -1;
	}

	if (isKeySet == false) {
		return -2;
	}

	if (desMode == ECB_MODE) {
		*out = ui64Des(block, false); 
	} else if (desMode == CBC_MODE) {
		lastBlock = ui64Des(block ^ lastBlock, false);
		*out = lastBlock;
	} else if (desMode == CFB_MODE) {
		lastBlock = ui64Des(lastBlock, false) ^ block;
		*out = lastBlock;
	}
	return 0;
}

int stdDes::ui64Decrypt(ui64 block, ui64 *out)
{
	if (out == NULL) {
		return -1;
	}

	if (isKeySet == false) {
		return -2;
	}

	if (desMode == ECB_MODE) {
		*out = ui64Des(block, true);
	} else if (desMode == CBC_MODE) {
		ui64 result = ui64Des(block, true) ^ lastBlock;
		lastBlock = block;
		*out = result;
	} else if (desMode == CFB_MODE) {
		ui64 result = ui64Des(lastBlock, false) ^ block;
		lastBlock = block;
		*out = result;
	}
	return 0;
}

int stdDes::encrypt(const void *inBuf, int inLen, void *outBuf, int outSize)
{
	unsigned char *pIn = (unsigned char *)inBuf;
	unsigned char *pOut = (unsigned char *)outBuf;

	if (inBuf == NULL || outBuf == NULL || outSize < inLen) {
		return -1;
	}

	if (inLen % 8 != 0) {
		return -2;
	}

	if (isKeySet == false) {
		return -3;
	}

	for (int i = 0; i < inLen / 8; i++) {
		ui64 block;
		ui64 blockOut = 0;

		block = bytesToBigEndianUi64(&pIn[8 * i], 8);
		if (ui64Encrypt(block, &blockOut) != 0) {
			return -4;
		}
		ui64ToBigEndianBytes(blockOut, &pOut[8 * i], 8);
	}

	return 0;
}

int stdDes::decrypt(const void *inBuf, int inLen, void *outBuf, int outSize)
{
	unsigned char *pIn = (unsigned char *)inBuf;
	unsigned char *pOut = (unsigned char *)outBuf;

	if (inBuf == NULL || outBuf == NULL || outSize < inLen) {
		return -1;
	}

	if (inLen % 8 != 0) {
		return -2;
	}

	if (isKeySet == false) {
		return -3;
	}

	for (int i = 0; i < inLen / 8; i++) {
		ui64 block;
		ui64 blockOut = 0;

		block = bytesToBigEndianUi64(&pIn[8 * i], 8);
		if (ui64Decrypt(block, &blockOut) != 0) {
			return -4;
		}
		ui64ToBigEndianBytes(blockOut, &pOut[8 * i], 8);
	}

	return 0;
}

void std3Des::setDesMode(E_DesMode mode, ui64 initVector)
{
	des1.setDesMode(mode, initVector);
	des2.setDesMode(mode, initVector);
	des3.setDesMode(mode, initVector);
}

int std3Des::setDesMode(E_DesMode mode, const unsigned char *initVector, int vecLen)
{
	int ret1, ret2, ret3;

	if (initVector == NULL || vecLen < 8) {
		return -1;
	}

	ret1 = des1.setDesMode(mode, initVector, vecLen);
	ret2 = des2.setDesMode(mode, initVector, vecLen);
	ret3 = des3.setDesMode(mode, initVector, vecLen);

	if (ret1 != 0 || ret2 != 0 || ret3 != 0) {
		return -2;
	}

	return 0;
}

void std3Des::resetIv()
{
	des1.resetIv();
	des2.resetIv();
	des3.resetIv();
}

void std3Des::setKey(ui64 key1, ui64 key2, ui64 key3)
{
	des1.setKey(key1);
	des2.setKey(key2);
	des3.setKey(key3);
}

int std3Des::setKey(const unsigned char *key, int keyLen)
{
	ui64 key1, key2, key3;

	if (key == NULL || keyLen < 16) {
		return -1;
	}

	key1 = stdDes::bytesToBigEndianUi64(key, 8);
	key2 = stdDes::bytesToBigEndianUi64(key+8, 8);

	if (keyLen < 24) {
		key3 = key1;
	} else {
		key3 = stdDes::bytesToBigEndianUi64(key+16, 8);
	}

	setKey(key1, key2, key3);
	return 0;
}

int std3Des::encrypt(const void *inBuf, int inLen, void *outBuf, int outSize)
{
	unsigned char *pIn = (unsigned char *)inBuf;
	unsigned char *pOut = (unsigned char *)outBuf;

	if (inBuf == NULL || outBuf == NULL || outSize < inLen) {
		return -1;
	}

	if (inLen % 8 != 0) {
		return -2;
	}

	if (des1.checkKeySet() == false || des2.checkKeySet() == false || des3.checkKeySet() == false) {
		return -3;
	}

	for (int i = 0; i < inLen / 8; i++) {
		ui64 block;
		ui64 ret1 = 0, ret2 = 0;
		ui64 blockOut = 0;

		block = stdDes::bytesToBigEndianUi64(&pIn[8 * i], 8);
		if (des1.ui64Encrypt(block, &ret1) != 0) {
			return -4;
		} 
		if (des2.ui64Decrypt(ret1, &ret2) != 0) {
			return -5;
		}
		if (des3.ui64Encrypt(ret2, &blockOut) != 0) {
			return -6;
		}
		stdDes::ui64ToBigEndianBytes(blockOut, &pOut[8 * i], 8);
	}

	return 0;
}

int std3Des::decrypt(const void *inBuf, int inLen, void *outBuf, int outSize)
{
	unsigned char *pIn = (unsigned char *)inBuf;
	unsigned char *pOut = (unsigned char *)outBuf;

	if (inBuf == NULL || outBuf == NULL || outSize < inLen) {
		return -1;
	}

	if (inLen % 8 != 0) {
		return -2;
	}

	if (des1.checkKeySet() == false || des2.checkKeySet() == false || des3.checkKeySet() == false) {
		return -3;
	}

	for (int i = 0; i < inLen / 8; i++) {
		ui64 block;
		ui64 ret1 = 0, ret2 = 0;
		ui64 blockOut = 0;

		block = stdDes::bytesToBigEndianUi64(&pIn[8 * i], 8);
		if (des3.ui64Decrypt(block, &ret1) != 0) {
			return -4;
		}
		if (des2.ui64Encrypt(ret1, &ret2) != 0) {
			return -5;
		}
		if (des1.ui64Decrypt(ret2, &blockOut) != 0) {
			return -6;
		}
		stdDes::ui64ToBigEndianBytes(blockOut, &pOut[8 * i], 8);
	}

	return 0;
}


}