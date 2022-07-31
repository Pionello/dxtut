#include <windows.h>
#include <windowsx.h>

LRESULT CALLBACK WinProc(HWND hWnd, 
	UINT message, 
	WPARAM wParam, 
	LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, // process handle
	HINSTANCE hPrevInstance,		    // prev process handle
	LPSTR lpCmdLine,					// additional launch parameters like "spore.exe -treeCreator"
	int nCmdShow)						// how to show the window (min/max active/inactive hide(background working)/show)
{
	// Create window class
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WinProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW - 2);
	//(HBRUSH)GetStockObject(BLACK_BRUSH); // - to black BG
	//COLOR_WINDOW;						   // - to white BG
	wc.lpszClassName = L"11A";

	RegisterClassEx(&wc);

	RECT wr = {0, 0, 500, 400}; // { widthStart, heightStart, widthFinish, heightFinish }
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	// Create a window of "wc" class 
	HWND hWnd = CreateWindowEx(NULL,   // win extended style
		wc.lpszClassName,              // win class; should be L"11A"
		L"My Window (just winapi so far)",                // win header
		WS_OVERLAPPEDWINDOW,           // win style
		200,						   // win position X
		100,						   // win position Y
		wr.right - wr.left,					       // win width
		wr.bottom - wr.top,						   // win height
		NULL,						   // parent window (none)
		NULL,						   // any menus (no)
		hInstance,					   // win process handle
		NULL);						   // multiple windows (no)

	// Show the window
	ShowWindow(hWnd, nCmdShow);

	// The main loop shows window until we close it:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}

	return msg.wParam;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch (message) 
	{
		case WM_DESTROY: 
		{
			PostQuitMessage(0);
			return 0;
		} break;
		default: break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

/*
* int WINAPI WinMain(HINSTANCE hInstance, // process handle
	HINSTANCE hPrevInstance,		      // prev process handle
	LPSTR lpCmdLine,					  // additional launch parameters like "spore.exe -treeCreator"
	int nCmdShow)						  // how to show the window: 
										  // (min/max active/inactive hide(background working)/show)
*/

/* MessageBox prototype:
	int MessageBox(HWND hWnd, // app id (guess it's hInstance)
		LPCTSTR lptext,       // message text
		LPCTSTR lpcaption,    // message header
		UINT utype); */       // answer buttons for user

/* CreateWindowEx prototype:
* CreateWindowEx(
    DWORD dwExStyle,		  // win extended style
    LPCWSTR lpClassName,	  // win class (16-bit string)
    LPCWSTR lpWindowName,	  // win title (16-bit string)
    DWORD dwStyle,			  // win style
    int X,					  // win X position
    int Y,					  // win Y position
    int nWidth,				  // win width
    int nHeight,			  // win height
    HWND hWndParent,		  // parent window
    HMENU hMenu,		      // win menus
    HINSTANCE hInstance,	  // win process handle (Handle to the instance of the module to be associated with the window)
    LPVOID lpParam);		  // Long pointer to a value to be passed to the window through the CREATESTRUCT structure 
							  // ...passed in the lParam parameter the WM_CREATE message.
*/

/*
	BOOL AdjustWindowRect(LPRECT lpRect, // rect with its size: { widthStart, heightStart, widthFinish, heightFinish }
                      DWORD dwStyle,	 // winstyle
                      BOOL bMenu);		 // using any menus
*/
