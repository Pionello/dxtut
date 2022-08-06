#include <windows.h>
#include <windowsx.h>
#include <stdio.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

CONST int C_WIDTH = 500;
CONST int C_HEIGHT = 400;
CONST LPCTSTR C_APPNAME = L"DXTUT";

float red = 0.0f;
float green = 0.0f;
float blue = 0.0f;
int colorCondition = 1; // delete it later



IDXGISwapChain* swapchain;			// pointer to swapchain interface
ID3D11Device* dev;					// pointer to device interface
ID3D11DeviceContext* devcon;		// pointer to device context
ID3D11RenderTargetView* backbuffer; // pointer to back buffer (swapchain of front- & back- buffers)

void InitD3D(HWND hWnd);			// Direct3D setup & initialization
void CleanD3D(void);				// Direct3D closing and memory release
void RenderFrame(void);

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
	WNDCLASSEX WinClass;
	ZeroMemory(&WinClass, sizeof(WNDCLASSEX));

	WinClass.cbSize = sizeof(WNDCLASSEX);
	WinClass.style = CS_HREDRAW | CS_VREDRAW;
	WinClass.lpfnWndProc = WinProc;
	WinClass.hInstance = hInstance;
	WinClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WinClass.hbrBackground = (HBRUSH)(COLOR_WINDOW - 2);
	//(HBRUSH)GetStockObject(BLACK_BRUSH); // - to black BG
	//COLOR_WINDOW;						   // - to white BG
	WinClass.lpszClassName = L"11A";

	RegisterClassEx(&WinClass);

	RECT WinRect = {0, 0, C_WIDTH, C_HEIGHT}; // { widthStart, heightStart, widthFinish, heightFinish }
	AdjustWindowRect(&WinRect,
		WS_OVERLAPPEDWINDOW, 
		FALSE);

	// Create a window of "wc" class 
	HWND hWnd = CreateWindowEx(NULL,   // win extended style
		WinClass.lpszClassName,        // win class; should be L"11A"
		C_APPNAME,					   // win header
		WS_OVERLAPPEDWINDOW,           // win style
		200,						   // win position X
		100,						   // win position Y
		WinRect.right - WinRect.left,  // win width
		WinRect.bottom - WinRect.top,  // win height
		NULL,						   // parent window (none)
		NULL,						   // any menus (no)
		hInstance,					   // win process handle
		NULL);						   // multiple windows (no)

	InitD3D(hWnd);

	ShowWindow(hWnd, nCmdShow);
	
	// The main loop shows window until we close it:
	MSG msg = {0};

	
	while (true) 
	{
		
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) break;
		}
		else 
		{
			// main app code
			switch (colorCondition) 
			{
				case 1: 
				{
					green += 0.1f;
					blue += 0.05f;

					if (green >= 1.0f) colorCondition = 2;
					break;
				}
				case 2:
				{
					green -= 0.1f;
					blue -= 0.05f;
					if (green <= 0.0f) colorCondition = 3;
					break;
				}
				case 3:
				{
					red += 0.1f;
					green += 0.05f;
					if (red >= 1.0f) colorCondition = 4;
					break;
				}
				case 4: 
				{
					red -= 0.1f;
					green -= 0.05f;
					if (red <= 0.0f) colorCondition = 5;
					break;
				}
				case 5:
				{
					blue += 0.1f;
					green += 0.05f;
					if (blue >= 1.0f) colorCondition = 6;
					break;
				}
				case 6:
				{
					blue -= 0.1f;
					green -= 0.05f;
					if (blue <= 0.0f) colorCondition = 1;
					break;
				}
				default:
				{
					break;
				}
			}
			Sleep(50);
			RenderFrame();
			// the main code of game/application
		}

	}

	CleanD3D();
	return msg.wParam;
}

void InitD3D(HWND hWnd) 
{
	// initialization itself
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 1;								// one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // how to use swapchain
	scd.OutputWindow = hWnd;							// the window
	scd.SampleDesc.Count = 4;							// how many multisamples (up to 4 supported)
	scd.Windowed = true;								// windowed mode
	scd.Flags = D3D11_CREATE_DEVICE_DEBUG;				// debug mode on

	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		scd.Flags,//D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&swapchain,
		&dev,
		NULL,
		&devcon);

	// backbuffer works (setting the render target)
	ID3D11Texture2D* pBackBuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer); // get certain (number 0 this case) 
																								 // backbuffer from swapchain and 
																								 // create ID3DTexture2D* using the buffer 

	dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);			 // put ID3DTexture2D* (pBackBuffer) 
																								 // to the backbuffer exactly
	pBackBuffer->Release(); // like for any COM object, release ID3D11Texture2D*

	devcon->OMSetRenderTargets(1, &backbuffer, NULL);		 // set the render target as backbuffer

	// setting the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = C_WIDTH;
	viewport.Height = C_HEIGHT;

	devcon->RSSetViewports(1, &viewport);


}

void CleanD3D()
{
	swapchain->Release();
	backbuffer->Release();
	dev->Release();
	devcon->Release();
}

void RenderFrame() 
{
	// clear backbuffer and fill it with a color
	devcon->ClearRenderTargetView(backbuffer, D3DXCOLOR(red, green, blue, 1.0f));

	// do 3D stuff here

	swapchain->Present(0, 0); // switch back n front buffers
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


// methods description
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

/*
* BOOL PeekMessage(LPMSG lpMsg, 
                 HWND hWnd,
                 UINT wMsgFilterMin,
                 UINT wMsgFilterMax,
                 UINT wRemoveMsg);
*/

/*
* HRESULT D3D11CreateDeviceAndSwapChain(
    IDXGIAdapter *pAdapter,
    D3D_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    D3D_FEATURE_LEVEL *pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,
    IDXGISwapChain **ppSwapChain,
    ID3D11Device **ppDevice,
    D3D_FEATURE_LEVEL *pFeatureLevel,
    ID3D11DeviceContext **ppDeviceContext);
*/
