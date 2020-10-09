#ifndef AES_H_20190809
#define AES_H_20190809

#include <stddef.h>
#include <string>

using namespace std;

namespace cbl{

//DES加密算法版本
enum E_AESVERSION {E_AESVERSION1=1, E_AESVERSION2, E_AESVERSION3, E_AESVERSION4, E_AESVERSION5};

class stdAes
{
public:
	stdAes() {isKeySet = false;}
	virtual ~stdAes() {}

	//设置秘钥，长度可为16,24,32字节,失败返回负数，成功返回0
	int setKey(const unsigned char *key, int keyLen);

	//加密数据，inLen必须是16的倍数，失败返回负数，成功返回0
	int encrypt(const void *inBuf, int inLen, void *outBuf, int outSize);

	//解密数据，inLen必须是16的倍数，失败返回负数，成功返回0
	int decrypt(const void *inBuf, int inLen, void *outBuf, int outSize);

private:
	void keyInit(int keyLen);
	unsigned char gadd(unsigned char a, unsigned char b);
	unsigned char gsub(unsigned char a, unsigned char b);
	unsigned char gmult(unsigned char a, unsigned char b);
	void coef_add(unsigned char a[], unsigned char b[], unsigned char d[]);
	void coef_mult(unsigned char *a, unsigned char *b, unsigned char *d);
	unsigned char* Rcon(unsigned char i);
	void add_round_key(unsigned char *state, unsigned char *w, unsigned char r);
	void mix_columns(unsigned char *state);
	void inv_mix_columns(unsigned char *state);
	void shift_rows(unsigned char *state);
	void inv_shift_rows(unsigned char *state);
	void sub_bytes(unsigned char *state);
	void inv_sub_bytes(unsigned char *state);
	void sub_word(unsigned char *w);
	void rot_word(unsigned char *w);

private:
	int Nk;
	int Nr;
	unsigned char w[240];

	//是否已经设置了key
	bool isKeySet; 
};

}

#endif