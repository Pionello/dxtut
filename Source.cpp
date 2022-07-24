#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	MessageBox(NULL, L"Alert!", L"Window header", MB_ICONEXCLAMATION | MB_OK);
	return 0;
}

