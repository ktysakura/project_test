#ifndef _SHELL_20191211_
#define _SHELL_20191211_
#include <iostream>
#include <string>
using namespace std;
namespace cbl{

int execShell(const string& sExeCmd, bool bShow = false);
int execShell(const string& sExeCmd, const string& sParam, bool bShow = false);

}


#endif