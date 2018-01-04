//////////////////////////////////////////////////////////////////////////////////////
//	Headers Section
//////////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include <stdlib.h>
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
int (WINAPIV * __vsnprintf)(char *, size_t, const char*, va_list) = _vsnprintf;
#include "camera.h"


//////////////////////////////////////////////////////////////////////////////////////
//	Global Variables
//////////////////////////////////////////////////////////////////////////////////////
HINSTANCE	g_hInst = NULL;
HWND		g_hWnd = NULL;

float g_clear_colour[4] = { 0.1f, 0.2f, 0.7f, 1.0f };

///Direct X Globals 
D3D_DRIVER_TYPE				g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL			g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*				g_pD3DDevice = NULL;
ID3D11DeviceContext*		g_pImmediateContext = NULL;
IDXGISwapChain*				g_pSwapChain = NULL;
ID3D11RenderTargetView*		g_pBackBufferRTView = NULL;

//Shader Variables
ID3D11Buffer*				g_pConstantBuffer0;
ID3D11Buffer*				g_pVertexBuffer;
ID3D11VertexShader*			g_pVertexShader;
ID3D11PixelShader*			g_pPixelShader;
ID3D11InputLayout*			g_pInputLayout;
ID3D11DepthStencilView*     g_pZBuffer;
ID3D11ShaderResourceView*	g_pTexture0;

// camera variables

Camera*						g_pMainCamera;
float						RotationSpeed = 100.0;
float						cameraSpeed = 1.0;

// Camera values
float MovementX = 0;
float MovementY = 0;
float MovementZ = 5;
int degree = 1;

//Define Vertex Shader

struct POS_COL_VERTEX
{
	XMFLOAT3 Pos;
	XMFLOAT4 Col;

};

//define Cbuffer

struct CONSTANT_BUFFER0
{		
	XMMATRIX WorldViewProjection; // 64 bytes (4x4 = 16 floats x4 bytes
	float RedAmount;		// 4 bytes
	float Scale;			// 4 bytes
	XMFLOAT2 packing_bytes; // 2x4 bytes = 8 bytes
};


//vertacies globals
POS_COL_VERTEX Vertices[] = {
	// back face 
	{XMFLOAT3(-1.0f, 1.0f, 1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(-1.0f, -1.0f, 1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(1.0f, 1.0f, 1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(1.0f, 1.0f, 1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(-1.0f, -1.0f, 1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(1.0f, -1.0f, 1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},

	// front face
	{XMFLOAT3(-1.0f, -1.0f, -1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(-1.0f, 1.0f, -1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(1.0f, 1.0f, -1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(-1.0f, -1.0f, -1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(1.0f, 1.0f, -1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(1.0f, -1.0f, -1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},

	// left face
	{XMFLOAT3(-1.0f, -1.0f, -1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(-1.0f, -1.0f, 1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(-1.0f, 1.0f, -1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(-1.0f, -1.0f, 1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(-1.0f, 1.0f, 1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(-1.0f, 1.0f, -1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},

	// right face
	{XMFLOAT3(1.0f, -1.0f, 1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(1.0f, -1.0f, -1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(1.0f, 1.0f, -1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(1.0f, 1.0f, 1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(1.0f, -1.0f, 1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(1.0f, 1.0f, -1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},

	// bottom face
	{XMFLOAT3(1.0f, -1.0f, -1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(1.0f, -1.0f, 1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(-1.0f, -1.0f, -1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(1.0f, -1.0f, 1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(-1.0f, -1.0f, 1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(-1.0f, -1.0f, -1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},

	// top face
	{XMFLOAT3(1.0f, 1.0f, 1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(1.0f, 1.0f, -1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(-1.0f, 1.0f, -1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(-1.0f, 1.0f, 1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(1.0f, 1.0f, 1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},
	{XMFLOAT3(-1.0f, 1.0f, -1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f)},

};


// Rename for each tutorial
char		g_TutorialName[100] = "Alex Bristow - Tutorial 07 Exercise 01\0";


//////////////////////////////////////////////////////////////////////////////////////
//	Forward declarations
//////////////////////////////////////////////////////////////////////////////////////
HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT InitialiseGraphics(void);
void RenderFrame(void);

///Direct X Forward declarations 
HRESULT InitialiseD3D();
void ShutdownD3D();



//////////////////////////////////////////////////////////////////////////////////////
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (FAILED(InitialiseWindow(hInstance, nCmdShow)))
	{
		DXTRACE_MSG("Failed to create Window");
		return 0;
	}
	if (FAILED(InitialiseD3D()))
	{
		DXTRACE_MSG("Failed to create Device");
		return 0;
	}

	if (FAILED(InitialiseGraphics()))
	{
		DXTRACE_MSG("Failed to Initialise Graphics");
		return 0;
	}


	// Main message loop
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			RenderFrame();// do something
		}
	}

	return (int)msg.wParam;
}


//////////////////////////////////////////////////////////////////////////////////////
// Register class and create window
//////////////////////////////////////////////////////////////////////////////////////
HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Give your app window your own name
	char Name[100] = "Alex Bristow\0";

	// Register class
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	//   wcex.hbrBackground = (HBRUSH )( COLOR_WINDOW + 1); // Needed for non-D3D apps
	wcex.lpszClassName = Name;

	if (!RegisterClassEx(&wcex)) return E_FAIL;

	// Create window
	g_hInst = hInstance;
	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(Name, g_TutorialName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left,
		rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	if (!g_hWnd)
		return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////////////
// Called every time the application receives a message
//////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	float x, y;
	x = LOWORD(lParam);
	y = HIWORD(lParam);

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == 0x57) // w key 
			g_pMainCamera->Forward(cameraSpeed);

		if (wParam == 0x53) // s key 
			g_pMainCamera->Forward(-cameraSpeed);
		if (wParam == 0x41)// d key
			g_pMainCamera->Rotate(RotationSpeed);

		if (wParam == 0x44)// a keyt 
			g_pMainCamera->Rotate(-RotationSpeed);

		if (wParam == VK_RETURN)
			degree = degree + 1;
		if (degree == 360)
		{
			degree = 0;
		};
			

		if (wParam == VK_ESCAPE)
		DestroyWindow(g_hWnd);
		return 0;
			
	case WM_MOUSEMOVE:
	
		break;

	case WM_LBUTTONDOWN:	
			
			
		break;

	case WM_RBUTTONDOWN:
	

		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
// Create D3D device and swap chain
//////////////////////////////////////////////////////////////////////////////////////
HRESULT InitialiseD3D()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(g_hWnd, &rc); // defines the rendable porportion of the window
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] = // array of all the possible drives to try and create and the order. 
	{
		D3D_DRIVER_TYPE_HARDWARE, // comment out this line if you need to test D3D 11.0 functionality on hardware that doesn't support it
		D3D_DRIVER_TYPE_WARP, // comment this out also to use reference device
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);
	D3D_FEATURE_LEVEL featureLevels[] =
	{
D3D_FEATURE_LEVEL_11_0,
D3D_FEATURE_LEVEL_10_1,
D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;										//Swap chain is a set of buffers Direct3D renders to then to screen 
	ZeroMemory(&sd, sizeof(sd));									//Standard dierctX way of initialising structurs 
	sd.BufferCount = 1;												//#No Back buffers in the swap chain
	sd.BufferDesc.Width = width;									//Sets the width of the rendable area 
	sd.BufferDesc.Height = height;									//Sets the hight of the rendable area
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				//Sets the format of the buffers 
	sd.BufferDesc.RefreshRate.Numerator = 60;						//Refresh rate 60
	sd.BufferDesc.RefreshRate.Denominator = 1;						//Refresh rate  1 =  60fps 
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;				//Where the swap chain buffers can be rendered to 
	sd.OutputWindow = g_hWnd;										//Which window Direct3D renders to
	sd.SampleDesc.Count = 1;										//Sets the antialiasing properties these values means no antialialising 
	sd.SampleDesc.Quality = 0;										//Sets the antialiasing properties these values means no antialialising 
	sd.Windowed = true;												//tells Direct3D to start in Windowed Mode

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)	//Loops through the spcified devices and attempts to create
	{																					//A device with associated swap chain based on feature levels
		g_driverType = driverTypes[driverTypeIndex];									//And swap chain 
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL,
			createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &g_pSwapChain,
			&g_pD3DDevice, &g_featureLevel, &g_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		return hr;

	// Get pointer to back buffer texture					
	ID3D11Texture2D *pBackBufferTexture;							//pointer to the Image Buffer  
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),		//GetBuffer() returns a pointer toi the back buffer of the swapchain in pBackBufferTexture
		(LPVOID*)&pBackBufferTexture);								//Identifies a texture object

	if (FAILED(hr)) return hr;


	// Use the back buffer texture pointer to create the render target view
	hr = g_pD3DDevice->CreateRenderTargetView(pBackBufferTexture, NULL,&g_pBackBufferRTView);													// creates a render target view from the pointer and returns it
	pBackBufferTexture->Release();

	//Resise buffers 
	g_pImmediateContext->OMGetRenderTargets(0, 0, 0);
	//Release all outstanding references to swap chain's buffers
	g_pBackBufferRTView->Release();


	//Preserve the existing buffer count and format
	//Automatically choose the width and hight to match the clint rect for HWNDS
	hr = g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

	//Error handling 
	if (FAILED(hr)) return hr;

	//get buffer and create a render target view
	ID3D11Texture2D* pBuffer;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);

	//Error handling 
	if (FAILED(hr)) return hr;

	//Set new new buffer target
	hr = g_pD3DDevice->CreateRenderTargetView(pBuffer, NULL, &g_pBackBufferRTView);
	//Error handling 
	if (FAILED(hr)) return hr;

	pBuffer->Release();

	//// Use the back buffer texture pointer to create the render target view
	hr = g_pD3DDevice->CreateRenderTargetView(pBackBufferTexture, NULL,&g_pBackBufferRTView);													// creates a render target view from the pointer and returns it
	//pBackBufferTexture->Release();

	if (FAILED(hr)) return hr;

	//create a Zbuffer texture 
	
	D3D11_TEXTURE2D_DESC tex2dDesc;
	ZeroMemory((&tex2dDesc), sizeof(tex2dDesc));

	tex2dDesc.Width = width;
	tex2dDesc.Height = height;
	tex2dDesc.ArraySize = 1;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex2dDesc.SampleDesc.Count = sd.SampleDesc.Count;
	tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D *pZBufferTexture;
	hr = g_pD3DDevice->CreateTexture2D(&tex2dDesc, NULL, &pZBufferTexture);
		//Error handling 
	if (FAILED(hr)) return hr;

	//create the z buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = tex2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	g_pD3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &g_pZBuffer);
	pZBufferTexture->Release();

	// Set the render target view
	g_pImmediateContext->OMSetRenderTargets(1, &g_pBackBufferRTView, g_pZBuffer);		//Sets the Render Target(usually 1)
																				//Second parameter is a pointer to a list of render target views.
																				//Third parameter is a list of depth/stencil views
	// Set the viewport
	D3D11_VIEWPORT viewport;

	viewport.TopLeftX = 0;														//Viewport Structure defines where in the render target to render
	viewport.TopLeftY = 0;														//Currently set to the same size the screen was set tp the same as the window
	viewport.Width = width;														//depth range used for special rendering techniques. Set to use full range 
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	g_pImmediateContext->RSSetViewports(1, &viewport);

	return S_OK;
}
//////////////////////////////////////////////////////////////////////////////////////
// Create D3D device and swap chain
//////////////////////////////////////////////////////////////////////////////////////
HRESULT InitialiseGraphics()
{
	HRESULT hr = S_OK;



	//set up and create the constant Buffer
	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));
	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT; // can use updatesubresroces() to update
	constant_buffer_desc.ByteWidth = 80; // MUSt be a mutilple of 16, calculate from the CB struct
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // use as a constant buffer
	hr = g_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &g_pConstantBuffer0);
	//error handling 
	if (FAILED(hr)) return hr;


	//Set up and create vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;										//This is used by the cpu and gpu
	bufferDesc.ByteWidth = sizeof(Vertices);							//Total Size of the Buffer
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;							//Use as a vertex Buffer 
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;							//Allow the cpu access
	hr = g_pD3DDevice->CreateBuffer(&bufferDesc, NULL, &g_pVertexBuffer);		//Create the Buffer

	//Error handling 
	if (FAILED(hr)) return hr;

	//copy the verticies into the buffer 
	D3D11_MAPPED_SUBRESOURCE ms;

	//lock the buffer to allow writing
	g_pImmediateContext->Map(g_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);

	//Copy the data
	memcpy(ms.pData, Vertices, sizeof(Vertices));

	//unlock the buffer
	g_pImmediateContext->Unmap(g_pVertexBuffer, NULL);

	//Load and compile vertex shaders - use  vs_5_0 to target dx11 hardware only
	ID3D10Blob *VS, *PS, *error;

	//hr = D3DX11CompileFromFileA("shaders.hlsl", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, &error, 0);
	hr = D3DX11CompileFromFile("shaders.hlsl", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, &error, 0);
		if (error != 0) //check for shader compliation error 
			{		OutputDebugStringA((char*)error->GetBufferPointer());
					error->Release();
					//Error handling 
				if (FAILED(hr)) return hr;
			}

	//Load and compile pixel  - use  vs_5_0 to target dx11 hardware only
	//hr = D3DX11CompileFromFileA("shaders.hlsl", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, &error, 0);

	hr = D3DX11CompileFromFile("shaders.hlsl", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, &error, 0);

		if (error != 0) //check for shader compliation error 
			{	
				OutputDebugStringA((char*)error->GetBufferPointer());
				error->Release();
			//Error handling 
			if (FAILED(hr)) return hr;
			}

	//create shader objects 
		hr = g_pD3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &g_pVertexShader);
		//Error handling 
			if (FAILED(hr)) return hr;

		hr = g_pD3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &g_pPixelShader);
		//Error handling 
			if (FAILED(hr)) return hr;

	//Set the shader object as active 
		g_pImmediateContext->VSSetShader(g_pVertexShader, 0, 0);
		g_pImmediateContext->PSSetShader(g_pPixelShader, 0, 0);

	//Create and set the input layout Object
		D3D11_INPUT_ELEMENT_DESC iedesc[] =
		{
			{"POSITION", 0 , DXGI_FORMAT_R32G32B32_FLOAT,0,0, D3D11_INPUT_PER_VERTEX_DATA,0},
			{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		};
	
		hr = g_pD3DDevice->CreateInputLayout(iedesc, 2, VS->GetBufferPointer(), VS->GetBufferSize(),&g_pInputLayout);
		//Error handling 
		if (FAILED(hr))
		{
			return hr;
		}

		g_pImmediateContext->IASetInputLayout(g_pInputLayout);

		g_pMainCamera = new Camera(0.0, 0.0, -10.5, 0.0);

		D3DX11CreateShaderResourceViewFromFile(g_pD3DDevice, "assets/texture.bmp", NULL, NULL, &g_pTexture0, NULL);
		
			return S_OK;

		
}
//////////////////////////////////////////////////////////////////////////////////////
// Render Frame
//////////////////////////////////////////////////////////////////////////////////////
void RenderFrame(void)
{

	CONSTANT_BUFFER0 cb0_values;
	cb0_values.RedAmount = 0.5f; // 50% of vertex value 
	cb0_values.Scale = 1.0f;//
	
	XMMATRIX projection, box_world, view;
	XMMATRIX box_world2;

	//world = XMMatrixTranslation(MovementX, MovementY, MovementZ);

	//world = XMMatrixRotationZ(XMConvertToRadians(45));
	//world *= XMMatrixTranslation(2, 0, 10);
	
	box_world = XMMatrixRotationX(XMConvertToRadians(degree));
	box_world *= XMMatrixTranslation(MovementX, MovementY, MovementZ);
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0), 640.0 / 480.0, 1.0, 100.0);
	
	view = g_pMainCamera->GetViewMatrix();
	
	
	// box 1

	//constant buffers information 
	cb0_values.WorldViewProjection = box_world * view * projection;
	//upload the new values for the cBuffer
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer0, 0, 0, &cb0_values, 0, 0);
	//seting the buffre to be active 
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer0);
	
	//Set Vertex Buffer
	UINT stride = sizeof(POS_COL_VERTEX);
	UINT offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	//Select which primitive type to use 
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	//used to tell the program how to draw the triangles 
	//draws the Deiveice contect to the back buffer
	//36 = The amount of vertices to draw
	//0 = where in the vertext buffer to start from,  0 = start 
	//Draw the vertex Buffer to the back buffer 
	g_pImmediateContext->Draw(36, 0);				


	//drawing box 2 
	box_world2 = XMMatrixRotationX(XMConvertToRadians(degree));
	box_world2 *= XMMatrixTranslation(0, 0, 10);
	
	cb0_values.WorldViewProjection = box_world2 *view * projection;
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer0, 0, 0, &cb0_values, 0, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer0);
	g_pImmediateContext->Draw(36, 0);

	// RENDER HERE

	// Display what has just been rendered
	g_pSwapChain->Present(0, 0);

	//Colour of the back buffer
	g_pImmediateContext->ClearRenderTargetView(g_pBackBufferRTView, g_clear_colour);
	g_pImmediateContext->ClearDepthStencilView(g_pZBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

}
//////////////////////////////////////////////////////////////////////////////////////
// Clean up D3D objects
//////////////////////////////////////////////////////////////////////////////////////
void ShutdownD3D()
{
	if (g_pMainCamera)			g_pMainCamera = NULL;
	if (g_pZBuffer)				g_pZBuffer->Release();								//Releases' the Z buffer	
	if (g_pVertexBuffer)		g_pVertexBuffer->Release();							//Releases' the Vertex Buffer 
	if (g_pInputLayout)			g_pInputLayout->Release();							//Releases' the Input Layout
	if (g_pVertexShader)		g_pVertexShader->Release();							//Releases' the Vertex Shader
	if (g_pPixelShader)			g_pPixelShader->Release();							//Releases' the Pixel Shader
	if (g_pConstantBuffer0)		g_pConstantBuffer0->Release();						//Releases' the Constant Buffer

	if (g_pSwapChain)			g_pSwapChain->Release();							//Releases' the Swap Chain  
	if (g_pImmediateContext)	g_pImmediateContext->Release();						//Releases' the Device Context 
	if (g_pD3DDevice)			g_pD3DDevice->Release();							//Releases' the Device
	if (g_pBackBufferRTView)	g_pBackBufferRTView->Release();						//Releases' the Back Buffer Render Target 
}