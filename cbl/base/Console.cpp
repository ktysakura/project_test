#include "Console.h"
#include <conio.h>
#include <stdio.h>
#include <windows.h>

namespace cbl {

CConsole::CConsole() 
{
	this->alloc();
}

CConsole::~CConsole()
{
	this->free();
}

int CConsole::alloc()
{
	HANDLE hConsole;
	HWND hWnd;
	HMENU hMenu;
	wchar_t szWindowTitle[256];

	if (!AllocConsole()) {
		return -1;
	}

	hConsole = CreateFileW(L"CONOUT$", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
	if (INVALID_HANDLE_VALUE == hConsole) {
		return -2;
	}

	GetConsoleTitleW(szWindowTitle, sizeof(szWindowTitle));
	hWnd = FindWindowW(L"ConsoleWindowClass", szWindowTitle);
	if (NULL == hWnd) {
		CloseHandle(hConsole);
		return -3;
	}

	hMenu = GetSystemMenu(hWnd, FALSE);
	if (hMenu) {
		DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
		DrawMenuBar(hWnd);
	}

	CloseHandle(hConsole);

	_wfreopen(L"CONOUT$", L"w", stdout);
	_wfreopen(L"CONOUT$", L"w", stderr);

	return 0;
}

void CConsole::free()
{
	FreeConsole();
}
	
}
