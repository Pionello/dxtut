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

	int test = MessageBox(NULL, // NULL - message comes from desktop, not any window
		L"Go?", 
		L"Main quest", 
		MB_ICONINFORMATION | MB_YESNO);

	switch (test) 
	{
		case (IDYES):MessageBox(NULL,
			L"GO",
			L"Info",
			MB_ICONEXCLAMATION | MB_OK);
			break;
		default: break;
	}
	return 0;
}

