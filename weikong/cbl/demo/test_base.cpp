#include "../src/cbl.h"
#include "main.h"

void test_base64()
{
	CBase64 base64;
	//CBase64 base64(true); //url模式

	unsigned char data[] = "\x11\x12\x13\x14\x15\x16\x17\x18\x00\x19\x20";
	char buf[100] = {0};
	string sDecode;
	int ret;

	//编码
	ret = base64.encode(data, sizeof(data) - 1, buf, sizeof(buf));
	cout << "encode ret: " << ret << endl;
	if (ret > 0) {
		cout << "encode result: " << buf << endl;
	}
	
	//解码
	ret = base64.decode(buf, ret, sDecode);
	cout << "decode ret: " << ret << endl;

	if (ret > 0) {
		printHex("decode result", sDecode.c_str(), sDecode.length());
		cout << endl;
	}
}

void test_bigEndian()
{
	U16 v1 = 0x1234;
	unsigned char buf[10] = {0};
	int ret;

	unsigned char *ptr = (unsigned char *)&v1;

	ret = setBigendianValueU16(v1, buf);
	if (ret < 0) {
		cout << "setBigendianValueU16 error" << endl;
	} else {
		printHex("", buf, 2);
	}

	U16 v2 = 0;
	ret = getBigendianValueU16(buf, &v2);
	if (ret < 0) {
		cout << "getBigendianValueU16 error" << endl;
	} else {
		printf("0x%x\n", v2);
	}
}

void test_crc()
{
	char buf[] = "123456789";
	unsigned long int c32;
	unsigned long long int c64;
	int c16;
	unsigned char xor;

	xor = calc_xor(buf, strlen(buf));
	printf("xor: %d\n", xor);

	c16 = crc16_table(buf, strlen(buf));
	cout << "crc16(table): " << c16 << endl;

	c16 = crc16_calc(buf, strlen(buf));
	cout << "crc16(calc): " << c16 << endl;

	c32 = crc32(buf, strlen(buf));
	cout << "crc32: " << c32 << endl;

	c64 = crc64(buf, strlen(buf));
	cout << "crc64: " << c64 << endl;
}

int onFindFile(const string &sDir, const WIN32_FIND_DATA_UTF8 &wfd, void *parameter)
{
	cout << utf8_to_gbk(sDir) << "\\" << utf8_to_gbk(wfd.cFileName) << endl;
	return 0;
}

void test_dir()
{
	int ret;
	list<DRIVE_INFO_T> dinfo;
	bool bret;

	ret = listDir(utf16_to_utf8(L"F:\\360云盘"), true, onFindFile, NULL);
	if (ret < 0) {
		cout << "listDir error" << endl;
	} 

	ret = getDriveInfo(&dinfo);
	if (ret < 0) {
		cout << "getDriveInfo error" << endl;
	} else {
		list<DRIVE_INFO_T>::iterator it;
		for (it = dinfo.begin(); it != dinfo.end(); it++) {
			cout << "sRootPath: " << utf8_to_gbk(it->sRootPath) << endl;
			cout << "sVolumeName: " << utf8_to_gbk(it->sVolumeName) << endl;
			cout << "sFileSystem: " << utf8_to_gbk(it->sFileSystem) << endl;
		}
	}
    
	bret = isDir(utf16_to_utf8(L"D:/work\\文档"));
	cout << "isDir: " << bret <<endl;

	cout << "getTempPathString: " << utf8_to_gbk(getTempPathString()) << endl;
	cout << "getModulePathString: " << utf8_to_gbk(getModulePathString()) << endl;

	bret = isPathFileExists(utf16_to_utf8(L"D:\\wecon/plc\\PLCEditor_5V"));
	cout << "isPathFileExists: " << bret << endl;

	bret = isPathFileExists(utf16_to_utf8(L"K:\\wecon\\plc\\PLCEditor_5V"));
	cout << "isPathFileExists: " << bret << endl;

	makeDir(utf16_to_utf8(L"F:\\abc\\def\\"));
	removeDir(utf16_to_utf8(L"F:\\abc/def/"));
}

void test_encode()
{
	const char *gbk = "\xce\xd2\xb5\xc4";
	const char *utf8 = "\xe6\x88\x91\xe7\x9a\x84";
	const char *utf16 = "\x11\x62\x84\x76";

	string sgbk;
	string sutf8;
	wstring sutf16;

	sutf8 = gbk_to_utf8(gbk);
	printHex("gbk_to_utf8", sutf8.c_str(), sutf8.length());

	sgbk = utf8_to_gbk(utf8);
	printHex("utf8_to_gbk", sgbk.c_str(), sgbk.length());

	sutf16 = gbk_to_utf16(gbk);
	printHex("gbk_to_utf16", (char *)sutf16.c_str(), sizeof(wchar_t) * sutf16.length());

	sgbk = utf16_to_gbk((wchar_t *)utf16);
	printHex("utf16_to_gbk", sgbk.c_str(), sgbk.length());

	sutf16 = utf8_to_utf16(utf8);
	printHex("utf8_to_utf16", (char *)sutf16.c_str(), sizeof(wchar_t) * sutf16.length());

	sutf8 = utf16_to_utf8((wchar_t *)utf16);
	printHex("utf16_to_utf8", sutf8.c_str(), sutf8.length());

	//下面的调用在中文windows系统等价于gbk_to_utf8
	sutf8 = utf16_to_utf8(acp_to_utf16("我的"));
	printHex("acp_to_utf8()", sutf8.c_str(), sutf8.length());
}

void test_file()
{
	list<string> lines;
	int ret;

	ret = readTextFile(utf16_to_utf8(L"C:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\include\\stdio.h"), &lines);
	if (ret > 0) {
		cout << "line count: " << ret << endl;
	}
}

void test_hightime()
{
	CHighTime ht;

	double v = ht.getTimeUs();
	cout << "timeus:" << v << endl;

	long long l = ht.getTimestamp();
	cout << "timestamp: " << l << endl;
}

void test_str()
{
	char buf[] = "		ddd   ";
	trim(buf);
	cout << "buf: " << buf << endl;

	string str1 = "aaaab\r";
	string str2 = removeCRLF(str1);
	cout << str2 << endl;

	string str3 = replaceall("111222333", "2", "44");
	cout << str3 << endl;
}

void test_base()
{
	test_base64();
	test_bigEndian();
	test_crc();
	test_dir();
	test_encode();
	test_file();
	test_hightime();
	test_str();

	getchar();
}
