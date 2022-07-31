#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, // process id
	HINSTANCE hPrevInstance, // prev process id
	LPSTR lpCmdLine, // additional launch parameters like "spore.exe -treeCreator"
	int nCmdShow) // how to show the window (min/max active/inactive hide(background working)/show)
{
	/* MessageBox prototype:
	int MessageBox(HWND hWnd, // app id (guess it's hInstance)
		LPCTSTR lptext,       // message text
		LPCTSTR lpcaption,    // message header
		UINT utype); */       // answer buttons for user

	int a = MessageBox(NULL, // NULL - message comes from desktop, not any window
		L"Not Hello World!",
		L"The error",
		MB_ICONERROR | MB_OK);



	return 0;
}

