#include "Base64.h"

namespace cbl {

static char s_base64_enc[64] = {
    'A','B','C','D','E','F','G','H','I','J','K','L','M',
    'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
    'a','b','c','d','e','f','g','h','i','j','k','l','m',
    'n','o','p','q','r','s','t','u','v','w','x','y','z',
    '0','1','2','3','4','5','6','7','8','9','+','/'
};

static char s_base64_url[64] = {
    'A','B','C','D','E','F','G','H','I','J','K','L','M',
    'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
    'a','b','c','d','e','f','g','h','i','j','k','l','m',
    'n','o','p','q','r','s','t','u','v','w','x','y','z',
    '0','1','2','3','4','5','6','7','8','9','-','_'
};

static const unsigned char s_base64_dec[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,62, 0,62, 0,63,		/* +/-/ */
    52,53,54,55,56,57,58,59,60,61, 0, 0, 0, 0, 0, 0,	/* 0 - 9 */
    0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,		/* A - Z */
    15,16,17,18,19,20,21,22,23,24,25, 0, 0, 0, 0,63,	/* _ */
    00,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,	/* a - z */
    41,42,43,44,45,46,47,48,49,50,51, 0, 0, 0, 0, 0,
};

CBase64::CBase64(bool urlMode)
{
	m_urlMode = urlMode;
}

CBase64::~CBase64()
{

}

int CBase64::encode(const void *source, size_t slen, char *target, size_t tlen)
{
	char *buf = NULL;
	int blen = slen * 2;
	size_t ret;

	if (source == NULL || slen == 0 || target == NULL || tlen == 0) {
		return -1;
	}
	
	buf = new char[blen];
	memset(buf, 0, blen);

	if (m_urlMode == true) {
		ret = base64_encode_table(buf, source, slen, s_base64_url);
	} else {
		ret = base64_encode_table(buf, source, slen, s_base64_enc);
	}
    
	if (ret > tlen) {
		delete[] buf;
		return -2;
	}

	memcpy(target, buf, ret);
	delete[] buf;
	return ret;
}

int CBase64::encode(const void *source, size_t slen, string &target)
{
	char *buf = NULL;
	int blen = slen * 2;
	size_t ret;	

	if (source == NULL || slen == 0) {
		return -1;
	}

	buf = new char[blen];
	memset(buf, 0, blen);

	if (m_urlMode == true) {
		ret = base64_encode_table(buf, source, slen, s_base64_url);
	} else {
		ret = base64_encode_table(buf, source, slen, s_base64_enc);
	}

	target.assign(buf, ret);
	delete[] buf;
	return ret;
}

int CBase64::decode(const char *source, size_t slen, void *target, size_t tlen)
{
	string result;
	int ret;

	if (source == NULL || slen == 0 || target == NULL || tlen == 0) {
		return -1;
	}

	ret = decode(source, slen, result);
	if (ret <= 0) {
		return -2;
	} else if (ret > (int)tlen) {
		return -3;
	}

	memcpy(target, result.data(), ret);
	return ret;
}

int CBase64::decode(const char *source, size_t slen, string &target)
{
    size_t i, j;
    unsigned char *buf = NULL;
    const unsigned char *start = (const unsigned char *)source;
    const unsigned char *end;

	if (source == NULL || slen % 4 != 0) {
		return -1;
	}

	buf = new unsigned char[slen];
	memset(buf, 0, slen);

    i = 0;
    end = start + slen;

    for (j = 1; j < slen / 4; j++) {
        buf[i++] = (s_base64_dec[start[0]] << 2) | (s_base64_dec[start[1]] >> 4);
        buf[i++] = (s_base64_dec[start[1]] << 4) | (s_base64_dec[start[2]] >> 2);
        buf[i++] = (s_base64_dec[start[2]] << 6) | s_base64_dec[start[3]];
        start += 4;
    }

    if (start + 4 == end) {
        buf[i++] = (s_base64_dec[start[0]] << 2) | (s_base64_dec[start[1]] >> 4);
		
		if ('=' != start[2]) {
            buf[i++] = (s_base64_dec[start[1]] << 4) | (s_base64_dec[start[2]] >> 2);
		}

		if ('=' != start[3]) {
            buf[i++] = (s_base64_dec[start[2]] << 6) | s_base64_dec[start[3]];
		}
    }

	target.assign((const char *)buf, i);
	delete[] buf;

    return i;
}

size_t CBase64::base64_encode_table(char *target, const void *source, size_t bytes, const char *table)
{
	size_t i, j;
	const unsigned char *ptr = (const unsigned char*)source;

	for (j = i = 0; i < bytes / 3 * 3; i += 3) {
		target[j++] = table[(ptr[i] >> 2) & 0x3F]; 
		target[j++] = table[((ptr[i] & 0x03) << 4) | ((ptr[i + 1] >> 4) & 0x0F)]; 
		target[j++] = table[((ptr[i + 1] & 0x0F) << 2) | ((ptr[i + 2] >> 6) & 0x03)];
		target[j++] = table[ptr[i + 2] & 0x3F];
	}

	if (i < bytes) {
		target[j++] = table[(ptr[i] >> 2) & 0x3F];
		if (i + 1 < bytes) {
			target[j++] = table[((ptr[i] & 0x03) << 4) | ((ptr[i + 1] >> 4) & 0x0F)];
			target[j++] = table[((ptr[i + 1] & 0x0F) << 2)];
		} else {
			target[j++] = table[((ptr[i] & 0x03) << 4)];
			target[j++] = '=';
		}
		target[j++] = '=';
	}

	return j;
}

}
