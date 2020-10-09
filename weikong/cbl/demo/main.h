#ifndef MAIN_H_20191218
#define MAIN_H_20191218

#include "../src/cbl.h"

void printHex(const string &tip, const void *ptr, size_t len, bool newLine = true);
void test_base();
void test_encrypt();
void test_excel();
void test_jsoncpp();
void test_tinyxml2();

#endif