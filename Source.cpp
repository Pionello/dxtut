#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	/* MessageBox prototype:
	int MessageBox(HWND hWnd,
		LPCTSTR lptext,
		LPCTSTR lpcaption,
		UINT utype); */

	MessageBox(NULL, // NULL - message comes from desktop, not any window
		L"R u happy?", 
		L"Main quest", 
		MB_ICONINFORMATION | MB_YESNO);
	return 0;
}

