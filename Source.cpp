#include <windows.h>
#include <windowsx.h>
#include <stdio.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

CONST int C_WIDTH = 1024;
CONST int C_HEIGHT = 768;
CONST LPCTSTR C_APPNAME = L"DXTUT";

float red =   0.0f;
float green = 0.1f;
float blue =  0.2f;
int colorCondition = 1; // delete it later
int VerticiesValue = 0;



IDXGISwapChain* swapchain;			// pointer to swapchain interface
ID3D11Device* dev;					// pointer to device interface
ID3D11DeviceContext* devcon;		// pointer to device context
ID3D11RenderTargetView* backbuffer; // pointer to back buffer (swapchain of front- & back- buffers)
ID3D11VertexShader* pVS;
ID3D11PixelShader* pPS;
ID3D11Buffer* pVBuffer;
ID3D11InputLayout* pLayout;

void InitD3D(HWND hWnd);			// Direct3D setup & initialization
void CleanD3D(void);				// Direct3D closing and memory release
void RenderFrame(void);
void InitGraphics(void);
void InitPipeline(void);


LRESULT CALLBACK WinProc(HWND hWnd, 
	UINT message, 
	WPARAM wParam, 
	LPARAM lParam);

HRESULT D3DX11CompileFromFile(
	LPCTSTR pSrcFile,
	D3D10_SHADER_MACRO* pDefines,
	LPD3D10INCLUDE pInclude,
	LPCSTR pFunctionName,
	LPCSTR pProfile,
	UINT Flags1,
	UINT Flags2,
	ID3DX11ThreadPump* pPump,
	ID3D10Blob** ppShader,
	ID3D10Blob** ppErrorMessages,
	HRESULT* pHResult
);

struct VERTEX 
{
	FLOAT X, Y, Z;
	D3DXCOLOR Color;
};

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
	//WinClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
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
	scd.BufferDesc.Width = C_WIDTH;						// set required width to the swapchain
	scd.BufferDesc.Height = C_HEIGHT;					// set required height to the swapchain
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // how to use swapchain
	scd.OutputWindow = hWnd;							// the window
	scd.SampleDesc.Count = 4;							// how many multisamples (up to 4 supported)
	scd.SampleDesc.Quality = 0;
	scd.Windowed = true;								// windowed mode
	scd.Flags = D3D11_CREATE_DEVICE_DEBUG | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;// debug mode on, alt+enter fullscreen swap 

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
	InitPipeline();
	InitGraphics();
}

void CleanD3D()
{
	
	pVS->Release();
	pPS->Release();
	pLayout->Release();
	swapchain->SetFullscreenState(false,NULL); // switch to windowed mode
	swapchain->Release();
	backbuffer->Release();
	dev->Release();
	devcon->Release();
}

void ColorSwapping() 
{
	switch (colorCondition)
	{
		case 1:
		{
			blue -= 0.05f;
			green -= 0.05f;
			if (green <= 0.0f) colorCondition = 2;
			break;
		}
		case 2:
		{
			blue += 0.05f;
			green += 0.05f;
			if (green >= 0.5f) colorCondition = 1;
			break;
		}
		default:
		{
			break;
		}
	}
}

void RenderFrame() 
{
	// clear backbuffer and fill it with a color
	devcon->ClearRenderTargetView(backbuffer, D3DXCOLOR(red, green, blue, 1.0f));

	ColorSwapping();
	Sleep(50);

	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
	devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devcon->Draw(VerticiesValue, 0);
	
	swapchain->Present(0, 0); // switch back n front buffers
}

void InitPipeline()
{
	// load & compile shaders
	ID3D10Blob* VS, * PS;
	D3DX11CompileFromFileA("shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, 0, 0);
	D3DX11CompileFromFileA("shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, 0, 0);

	// encapsulate shaders into shader objects
	dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
	dev->CreatePixelShader (PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

	// set shader objects
	devcon->VSSetShader(pVS, 0 ,0);
	devcon->PSSetShader(pPS, 0, 0);

	// input layout object
	D3D11_INPUT_ELEMENT_DESC ied[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	dev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
	devcon->IASetInputLayout(pLayout);
}

void InitGraphics() 
{
	//VERTEX JustVertex = { 0.0f, 0.5f, 0.0f, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) };

	VERTEX VertexArray[] =
	{
		{ 0.0f, -0.5f, 0.0f, D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f)},
		{ 0.0f,  0.5f, 0.0f, D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f)},
		{ 0.5f, -0.3f, 0.0f, D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f)},

		{ 0.5f, -0.3f, 0.0f, D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f)},
		{ 0.0f,  0.5f, 0.0f, D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f)},
		{ 0.5f,  0.6f, 0.0f, D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f)},

		{-0.5f, -0.3f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)},
		{-0.5f,  0.6f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)},
		{ 0.0f, -0.5f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)},

		{ 0.0f, -0.5f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)},
		{-0.5f,  0.6f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)},
		{ 0.0f,  0.5f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)},

		{ 0.0f,  0.4f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.8f, 1.0f)},
		{-0.5f,  0.6f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.8f, 1.0f)},
		{ 0.0f,  0.8f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.8f, 1.0f)},

		{ 0.0f,  0.8f, 0.0f, D3DXCOLOR(0.0f, 0.8f, 0.8f, 1.0f)},
		{ 0.5f,  0.6f, 0.0f, D3DXCOLOR(0.0f, 0.8f, 0.8f, 1.0f)},
		{ 0.0f,  0.4f, 0.0f, D3DXCOLOR(0.0f, 0.8f, 0.8f, 1.0f)}


		//{ 0.5f,  0.5f, 0.0f, D3DXCOLOR(0.5f, 1.0f, 0.0f, 1.0f)},
		//{-0.5f, -0.5f, 0.0f, D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f)},
		//{ 0.5f, -0.5f, 0.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)}
		
	};

	VerticiesValue = sizeof(VertexArray) / sizeof(VertexArray[0]);

	// buffer code
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX) * VerticiesValue; // to check (3)
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	dev->CreateBuffer(&bd, NULL, &pVBuffer);

	// buffer mapping
	D3D11_MAPPED_SUBRESOURCE ms;
	ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE)); // to check
	devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, VertexArray, sizeof(VertexArray));
	devcon->Unmap(pVBuffer, NULL);
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
