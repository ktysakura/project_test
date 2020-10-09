#include "../src/cbl.h"
#include "main.h"

void test_des(bool cfb)
{
	stdDes des;
	int ret;
	unsigned char out[20] = {0};
	unsigned char out2[20] = {0};

	if (cfb) {
		des.setDesMode(CFB_MODE, 0x0000000000000000);
	}

	//设置key
	ret = des.setKey((unsigned char *)"\x12\x34\x56\x78\x90\xab\xcd\xef", 8);
	if (ret < 0) {
		printf("setKey failed\n");
		return;
	}	

	//加密
	ret = des.encrypt((void *)"\x94\x74\xB8\xE8\xC7\x3B\xCA\x7D\x94\x74\xB8\xE8\xC7\x3B\xCA\x7D", 16, out, 20);
	printHex("des encrypt", out, 16);
	
	if (cfb) {
		des.resetIv();
	}

	//解密
	des.decrypt(out, 16, out2, 20);
	printHex("des decrypt", out2, 16);
}

void test_3des(bool cfb)
{
	std3Des des;
	int ret;
	unsigned char out[20] = {0};
	unsigned char out2[20] = {0};

	if (cfb) {
		des.setDesMode(CFB_MODE, 0x0000000000000000);
	}

	//设置key
	ret = des.setKey((unsigned char *)"\x12\x34\x56\x78\x90\xab\xcd\xef\x12\x34\x56\x78\x90\xab\xcd\xef", 16);
	if (ret < 0) {
		printf("setKey failed\n");
		return;
	}	

	//加密
	des.encrypt((void *)"\x94\x74\xB8\xE8\xC7\x3B\xCA\x7D\x94\x74\xB8\xE8\xC7\x3B\xCA\x7D", 16, out, 20);
	printHex("3des encrypt", out, 16);

	if (cfb) {
		des.resetIv();
	}

	//解密
	des.decrypt(out, 16, out2, 20);
	printHex("3des decrypt", out2, 16);
}

void test_aes(int keybit)
{
	// 128 bit key 
	unsigned char key1[] = {
		0x00, 0x01, 0x02, 0x03, 
		0x04, 0x05, 0x06, 0x07, 
		0x08, 0x09, 0x0a, 0x0b, 
		0x0c, 0x0d, 0x0e, 0x0f
	}; 

	/* 192 bit key */
	unsigned char key2[] = {
		0x00, 0x01, 0x02, 0x03,
		0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b,
		0x0c, 0x0d, 0x0e, 0x0f,
		0x10, 0x11, 0x12, 0x13,
		0x14, 0x15, 0x16, 0x17
	}; 

	/* 256 bit key */
	unsigned char key3[] = {
		0x00, 0x01, 0x02, 0x03,
		0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b,
		0x0c, 0x0d, 0x0e, 0x0f,
		0x10, 0x11, 0x12, 0x13,
		0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0x1a, 0x1b,
		0x1c, 0x1d, 0x1e, 0x1f
	};

	unsigned char in[] = {
		0x00, 0x11, 0x22, 0x33,
		0x44, 0x55, 0x66, 0x77,
		0x88, 0x99, 0xaa, 0xbb,
		0xcc, 0xdd, 0xee, 0xff
	};

	unsigned char out[16];
	int i;
	stdAes aes;

	if (keybit == 128) {
		aes.setKey(key1, sizeof(key1));
	} else if (keybit == 192) {
		aes.setKey(key2, sizeof(key2));
	} else if (keybit == 256) {
		aes.setKey(key3, sizeof(key3));
	} else {
		return;
	}
	
	printf("Plaintext message:\n");
	for (i = 0; i < 4; i++) {
		printf("%02x %02x %02x %02x ", in[4*i+0], in[4*i+1], in[4*i+2], in[4*i+3]);
	}

	printf("\n");

	aes.encrypt(in, 16, out, 16);

	printf("encrypt message:\n");
	for (i = 0; i < 4; i++) {
		printf("%02x %02x %02x %02x ", out[4*i+0], out[4*i+1], out[4*i+2], out[4*i+3]);
	}

	printf("\n");

	aes.decrypt(out, 16, in, 16);

	printf("decrypt message:\n");
	for (i = 0; i < 4; i++) {
		printf("%02x %02x %02x %02x ", in[4*i+0], in[4*i+1], in[4*i+2], in[4*i+3]);
	}

	printf("\n\n");
}

void test_rc4()
{
	CRc4 rc4;
	unsigned char key[] = {
		0x00, 0x01, 0x02, 0x03,
		0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b,
		0x0c, 0x0d, 0x0e, 0x0f,
	};

	unsigned char in[] = {
		0x00, 0x11, 0x22, 0x33,
		0x44, 0x55, 0x66, 0x77,
		0x88, 0x99, 0xaa, 0xbb,
		0x88, 0x99, 0xaa, 0xbb,
		0xcc, 0xdd, 0xee, 0xff,
		0xcc, 0xdd, 0xee, 0xff,
		0x88, 0x99, 0xaa, 0xbb,
		0xcc, 0xdd, 0xee, 0xff,
		0x00, 0x11, 0x22, 0x33,
		0x44, 0x55, 0x66, 0x77,
		0x88, 0x99, 0xaa, 0xbb,
		0x88, 0x99, 0xaa, 0xbb,
		0xcc, 0xdd, 0xee, 0xff,
		0xcc, 0xdd, 0xee, 0xff,
		0x88, 0x99, 0xaa, 0xbb,
		0xcc, 0xdd, 0xee, 0xff
	};
	unsigned char *out = new unsigned char[sizeof(in)];
	unsigned char *din = new unsigned char[sizeof(in)];

	rc4.setKey(key, sizeof(key));
	printHex("original msg", in, sizeof(in));


	rc4.encrypt(in, sizeof(in), out);
	printHex("encrypt msg", out, sizeof(in));

	rc4.decrypt(out, sizeof(in), din);
	printHex("decrypt msg", din, sizeof(in));

	delete[] out;
	delete[] din;
}

void test_encrypt()
{
	test_des(false);
	test_des(true);

	test_3des(false);
	test_3des(true);

	cout << "128 bits: " << endl;
	test_aes(128);
	cout << "192 bits: " << endl;
	test_aes(192);
	cout << "256 bits: " << endl;
	test_aes(256);

	test_rc4();

	getchar();
}