#include "rc4.h"

namespace cbl {

int CRc4::setKey(const void *key, size_t keyLen)
{
	unsigned char *pKey = (unsigned char *)key;

    if (pKey == NULL) {
        return -1;
    }

    m_keyLen = keyLen;
	if (m_keyVec.size() > 0) {
		m_keyVec.clear();
	}

    for (size_t i = 0; i < keyLen; ++i) {
        m_keyVec.push_back(pKey[i]);
    }

    return 0;
}

int CRc4::getKeyStream(size_t dataLen, unsigned char *keyStream)
{
	size_t i = 0, j = 0, t = 0;

	if (dataLen == 0 || keyStream == NULL) {
		return -1;
	}

	for (i = 0; i < 256; ++i) {
		S[i] = i;
		T[i] = m_keyVec[i % m_keyLen];
	}

	for (i = 0; i < 256; ++i) {
		j = (j + S[i] + T[i]) % 256;
		std::swap(S[i], S[j]);
	}

	for (size_t k = 0; k < dataLen; ++k) {
		i = (i + 1) % 256;
		j = (j + S[i]) % 256;
		std::swap(S[i], S[j]);

		t = (S[i] + S[j]) % 256;
		keyStream[k] = S[t];
	}

	return 0;
}

int CRc4::encrypt(const void *in, size_t inLen, void *out)
{
	unsigned char *pStream = NULL;
	unsigned char *pIn = (unsigned char *)in;
	unsigned char *pOut = (unsigned char *)out;

    if (in == NULL || inLen == 0 || out == NULL) {
        return -1;
    }

	if (m_keyLen == 0 || m_keyVec.size() == 0) {
		return -2;
	}

	pStream = new unsigned char[inLen];
	if (pStream == NULL) {
		return -3;
	}
	memset(pStream, 0, inLen);
	
	if (getKeyStream(inLen, pStream) != 0) {
		delete[] pStream;
		pStream = NULL;
		return -4;
	}

    for (size_t k = 0; k < inLen; ++k) {
        pOut[k] = pStream[k] ^ pIn[k];
    }

	if (pStream) {
		delete[] pStream;
		pStream = NULL;
	}

    return 0;
}

int CRc4::decrypt(const void *in, size_t inLen, void *out)
{
	unsigned char *pStream = NULL;
	unsigned char *pIn = (unsigned char *)in;
	unsigned char *pOut = (unsigned char *)out;

    if (in == NULL || inLen == 0 || out == NULL) {
        return -1;
    }

	if (m_keyLen == 0 || m_keyVec.size() == 0) {
		return -2;
	}

	pStream = new unsigned char[inLen];
	if (pStream == NULL) {
		return -3;
	}
	memset(pStream, 0, inLen);

	if (getKeyStream(inLen, pStream) != 0) {
		delete[] pStream;
		pStream = NULL;
		return -4;
	}

    for (size_t k = 0; k < inLen; ++k) {
        pOut[k] = pStream[k] ^ pIn[k];
    }

	if (pStream) {
		delete[] pStream;
		pStream = NULL;
	}

	return 0;
}

}