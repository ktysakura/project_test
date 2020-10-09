#include "main.h"


int main(int argc, char *argv[])
{
	test_base();
	//test_encrypt();
	//test_excel();
	//test_jsoncpp();
	//test_tinyxml2();
	return 0;
}

void printHex(const string &tip, const void *ptr, size_t len, bool newLine)
{
	const unsigned char *buf = (const unsigned char *)ptr;

	if (ptr == NULL || len == 0) {
		return;
	}

	cout << tip << ": ";
	for (size_t i = 0; i < len; i++) {
		printf("%02X ", buf[i]);
	}

	if (newLine) {
		cout << endl;
	}
}