#ifndef DES_H_20190808
#define DES_H_20190808

#include <string>

using namespace std;

namespace cbl {

typedef unsigned long long int	ui64;
typedef unsigned int			ui32;
typedef unsigned char			ui8;

//DES加密算法模式
enum E_DesMode {ECB_MODE, CBC_MODE, CFB_MODE};

//DES加密算法版本
enum E_DESVERSION {E_DESVERSION1=1, E_DESVERSION2, E_DESVERSION3, E_DESVERSION4, E_DESVERSION5};

//这是一个标准的des类
class stdDes
{
public:
	stdDes() {desMode = ECB_MODE; isKeySet = false;}
	virtual ~stdDes() {}

	//设置CBC/CFB模式的初始向量，ECB无需iv
	void setDesMode(E_DesMode mode, ui64 initVector);

	//设置CBC/CFB模式的初始向量，ECB无需iv，取initVector前8个字节作为初始向量
	int setDesMode(E_DesMode mode, const unsigned char *initVector, int vecLen);

	//复位CBC/CFB的iv
	void resetIv();

	//设置DES秘钥
	void setKey(ui64 key);

	//设置DES秘钥，keyLen必须大于等于8，只取前8个字节作为秘钥，成功返回0
	int setKey(const unsigned char *key, int keyLen);
	
	//加密一个8字节的整数，成功返回0
	int ui64Encrypt(ui64 block, ui64 *out);

	//解密一个8字节的整数，成功返回0
	int ui64Decrypt(ui64 block, ui64 *out);

	//加密指定长度的字节数组，inLen必须为8的倍数，成功返回0
	int encrypt(const void *inBuf, int inLen, void *outBuf, int outSize);

	//解密指定长度的字节数组，inLen必须为8的倍数，成功返回0
	int decrypt(const void *inBuf, int inLen, void *outBuf, int outSize);

	static ui64 strToBigEndianUi64(const char *stringKey);
	static ui64 bytesToBigEndianUi64(const unsigned char *bytes, int bytesLen);
	static int ui64ToBigEndianBytes(ui64 value, unsigned char *out, int outLen);

	//判断是否已经设置了key
	bool checkKeySet() {return isKeySet;}

private:
	ui64 ui64Des(ui64 block, bool mode);

	void keygen(ui64 key);
	ui64 ip(ui64 block);
	ui64 fp(ui64 block);
	void feistel(ui32 &L, ui32 &R, ui32 F);
	ui32 f(ui32 R, ui64 k);

private:
	ui64 sub_key[16];
	E_DesMode desMode;
	ui64 iv;
	ui64 lastBlock;

	//是否已经设置了key
	bool isKeySet; 
};

//这是一个标准的3des类
class std3Des
{
public:
	std3Des() {}
	virtual ~std3Des() {}

	//设置CBC/CFB模式的初始向量，ECB无需iv
	void setDesMode(E_DesMode mode, ui64 initVector);

	//设置CBC/CFB模式的初始向量，ECB无需iv，取initVector前8个字节作为初始向量
	int setDesMode(E_DesMode mode, const unsigned char *initVector, int vecLen);

	//复位CBC/CFB的iv
	void resetIv();

	//设置3DES秘钥
	void setKey(ui64 key1, ui64 key2, ui64 key3);

	//设置3DES秘钥，keyLen必须大于等于16，只取前16或24个字节作为秘钥，成功返回0
	int setKey(const unsigned char *key, int keyLen);

	//加密指定长度的字节数组，inLen必须为8的倍数，成功返回0
	int encrypt(const void *inBuf, int inLen, void *outBuf, int outSize);

	//解密指定长度的字节数组，inLen必须为8的倍数，成功返回0
	int decrypt(const void *inBuf, int inLen, void *outBuf, int outSize);

private:
	stdDes des1;
	stdDes des2;
	stdDes des3;
};
}

#endif
