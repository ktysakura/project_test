#ifndef PI_RC4_H_20190824
#define PI_RC4_H_20190824

#include <vector>
#include <string.h>

using namespace std;

namespace cbl {

class CRc4
{
public:
	//设置秘钥，成功返回0
	int setKey(const void *key, size_t keyLen);

	//加密函数，成功返回0
	int encrypt(const void *in, size_t inLen, void *out);

	//解密函数，成功返回0
	int decrypt(const void *in, size_t inLen, void *out);

private:
	int getKeyStream(size_t dataLen, unsigned char *keyStream);

private:
	int m_keyLen;
	vector<unsigned char> m_keyVec;
	unsigned char S[256];
	unsigned char T[256];
};

}

#endif