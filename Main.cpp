
/* --------------------------------

Welcome to the source code for
'Mowve It' - the lawn mowing game.

The following code is written in the
C++ programming language and is
compiled into a Portable Executable
(.exe) using Microsoft Visual C++ 2010.

Written by Callum Morgan.

Any text contained within the dash and
asterisk tokens are 'commentary' and do
not affect the result of the compilation.
They are used for the benefit of the viewer
of the code or to guide the editing programmer.

Any text that follows the double forward dashes
token ('//') are also 'commentary' and are not
part of the source code. Any text following and
on the same line as the double forward dashes are
ignored by the compiler during compilation.

-------------------------------- */

#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif
#ifndef DIRECTSOUND_VERSION
#define DIRECTSOUND_VERSION 0x1000
#endif

/* The hashtag token indicates a compiler
predirective command.
'#include "Resource.h"' declares that the source file
contained within the quotations must be included
during the compilation process.

Contained within GOBJ.h are more #include directives
which allows the inclusion of more files that will
compile with the project. */
#include "Resource.h"
#include "GameResource.h"
#include "CStruct.h"
#include "GameObj.h"

/* --------------------------------

Include file for 'Mowve It' game project.

Following are include files which must be included in the
compilation of this project.

'cstdlib' is an abbreviation of 'C Standard Library'. This
allows the C++ programmer to access features provided by the
library belonging to the C Standard.

'cstdio' is an abbreviated form of 'C Standard Input/Output'
'cstring' substitutes for 'C String'
'Windows.h' is a 'header' file which follows the same
structure as ordinary include files that don't have a
file extension.
'MMSystem.h' is an API for handling multimedia devices.
'd3d9.h' is an abbrevation of 'Direct 3D 9' which is
a Application Programming Interface (API) - in this case a
library of functions provided by Microsoft to allow
the software engineer to utilize high-performance
3D (and 2D) graphics rendering features.
'dsound.h' is an abbreviation of 'Direct Sound' which
is a library utility provided by Microsoft to implement
high-performance sound manipulation in the software.
'new' is a C++ feature which handles the usage of
dynamic runtime memory and class abstraction.
'crtdbg.h' includes functionality for debugging
the program heap. Mainly used to track memory leaks
if or when they occur.

-------------------------------- */

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <Windows.h>
#include <MMSystem.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dsound.h>
#include <new>
#include <crtdbg.h>

/* C++ requires '.lib' files to describe the properties
of functions that may be imported from Dynamic Link Libraries
('.dll'). The following lines declare that these libraries are
a dependency. */
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")

/* These lines disable the corresponding warning messages that
pollute the build output. */
#pragma warning(disable:4996)
#pragma warning(disable:4345)

/* For DEBUG builds only - this line of text is displayed in the
Main Menu. This shows the current version of MowveIt. */
#define MOWVE_IT_VERSION "\"Mowve It.exe\" build version 1.0.0\n02/04/2015"

/* Following is a declaration and definition of global
variables involved in managing the game. */
GOBJ_TimeTracker		g_Time;
HANDLE					ProcHeap;
char *					WindowTitle		= "Mowve It"; // Displayed in the title bar
char *					WindowClass		= "MowveItGame"; // Identifies the window class
HCURSOR					g_CArrow;
HCURSOR					g_CSelect;
RECT					g_ClientRect;
RECT					g_WindowRect;
float					g_dX;
float					g_dY;
HINSTANCE				g_hInst			= nullptr; // Instance handle to the current window process
HWND					g_hWnd			= nullptr; // Handle to the actual window form itself
MOUSE_STATE				g_Mouse;
KEYBOARD_STATE			g_Keyboard;

IDirect3D9 *			g_pD3D			= nullptr;
IDirect3DDevice9 *		g_pd3dDevice	= nullptr;
IDirectSound *			g_pSound		= nullptr;
DWORD					g_NumSamples; // Multisampling

ID3DXFont *				g_Font			= nullptr;
ID3DXSprite *			g_Sprite		= nullptr;

float					g_AspectRatio;
DWORD					g_Ambient		= 0x4080f0;

CCamera					g_Camera;
bool					g_CamFollow		= false;

Queue					g_Queue;
ResourceManager			g_Resource;
GOBJ_CONTEXT*			g_pContext		= nullptr;

float					g_MusicVolume	= 1.0f;
DWORD					g_GrassDensity	= IDR_STR_Grass1;



/* Following is a declaration (not definition) of global
functions involved in managing the game. */
int		__stdcall	WinMain(HINSTANCE,HINSTANCE,LPSTR,int);
LRESULT	__stdcall	WndProc(HWND,UINT,WPARAM,LPARAM);
int		Cleanup();
int		InitD3D();
int		InitDSound();

int __stdcall CreateMainMenu();
int __stdcall CreateHelpScreen();
int __stdcall CreateOptionsMenu();
int __stdcall CreatePauseMenu();
int __stdcall PauseMenuRestorePrevious();

int __stdcall CreateLevel1();
int __stdcall CreateLevel2();
int __stdcall CreateLevel3();
int __stdcall CreateLevel4();
int __stdcall CreateEndGame();

int __stdcall SwitchToMowerMini();
int __stdcall SwitchToMowerMover();
int __stdcall SwitchToMowerMonster();

HRESULT LoadEmbeddedWAV(Resource_Sound *, LPSTR);
HRESULT LoadEmbeddedMesh(Resource_Mesh *, LPSTR);
DWORD GetResourceIntByName( LPSTR );



/* Following is the definition (not declaration) of one of
the globally declared functions above. Functions perform
actions. */
int __stdcall WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd )
{
	// Enable heap debug functionality
#ifdef DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// Get process heap
	ProcHeap = GetProcessHeap();

	// Store instance handle in our global variable.
	g_hInst = hInstance;

	// Register class.
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor        = g_CArrow = LoadCursor(NULL, IDC_ARROW);
	g_CSelect			= LoadCursor(NULL, IDC_HAND);
	wcex.hbrBackground  = 0;
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = WindowClass;
	wcex.hIconSm        = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	if( !RegisterClassEx(&wcex) )
		return EXIT_FAILURE;

	// Get screen size
	g_ClientRect.right  = GetSystemMetrics(SM_CXSCREEN);
	g_ClientRect.bottom = GetSystemMetrics(SM_CYSCREEN);

	// Adjust rectangles
	g_WindowRect = g_ClientRect;
	AdjustWindowRect( &g_WindowRect, WS_POPUP, FALSE );

	// Create window
	g_hWnd = CreateWindowA( WindowClass, WindowTitle,
		WS_POPUP | WS_VISIBLE,
		g_WindowRect.left, g_WindowRect.top,
		g_WindowRect.right - g_WindowRect.left,
		g_WindowRect.bottom - g_WindowRect.top,
		NULL, NULL, hInstance, NULL);

	// If the window could not be created, terminate the process
	if( !g_hWnd )
		return EXIT_FAILURE;

	// Initialize sound device.
	if( FAILED( InitDSound() ) )
	{
		/* If no devices are supported, report that the process
		cannot start up and then terminate. */
		MessageBoxA( g_hWnd,
			"Failed to initialise DirectSound.\n"
			"Please install/re-install DirectSound to continue.\n\n"
			"The process will now terminate.",
			WindowTitle, MB_ICONERROR );
		DestroyWindow(g_hWnd);
		Cleanup();
		return EXIT_FAILURE;
	}

	// Initialize rendering device.
	if( FAILED( InitD3D() ) )
	{
		/* If no devices are supported, report that the process
		cannot start up and then terminate. */
		MessageBoxA( g_hWnd,
			"Direct3D 9 is unsupported.\n"
			"This may require a reinstallation of DirectX version 9.0 to continue.\n\n"
			"The process will now terminate.",
			"Start up failure.", MB_ICONERROR );
		DestroyWindow(g_hWnd);
		Cleanup();
		return EXIT_FAILURE;
	}

	// Initialize game
	if( FAILED( CreateMainMenu() ) )
	{
		MessageBoxA( g_hWnd,
			"Unknown startup error.",
			"Start up failure.", MB_ICONERROR );
		DestroyWindow(g_hWnd);
		Cleanup();
		return EXIT_FAILURE;
	}

	// Set random seed
	srand( GetTickCount() );

	// Initialize g_Time
	g_Time.Initialise();

	// Play loop
	{
		Resource_Sound *pLoop = (Resource_Sound *)
			g_Resource.GetResourceByName("SndLoop");
		if( pLoop )
		{
			pLoop->pBuffer->SetCurrentPosition(0);
			pLoop->pBuffer->Play(0, 0, DSBPLAY_LOOPING);
		}
	}

	// Enter main message loop.
	MSG msg = {0};
	while( msg.message != WM_QUIT )
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			DispatchMessage( &msg );
		else
		{
			// Process requests
			g_Queue.Execute();

			// Get time
			g_Time.Update();

			// Update camera
			g_Camera.Update();

			// Get position of the cursor
			GetCursorPos(&g_Mouse.Position);
			ScreenToClient(g_hWnd,&g_Mouse.Position);

			// If one is available, signal events
			if( g_pContext )
			{
				// Process mouse input
				g_pContext->Mouse();
		
				// Get keys
				GetKeyboardState( g_Keyboard.Keys );

				// Process keyboard input
				g_pContext->Keyboard();

				// Signal update
				g_pContext->Update();

				// Is device valid?
				if( g_pd3dDevice )
				{
					// Begin rendering
					g_pd3dDevice->BeginScene();

					// Render
					g_pContext->Render();

					// End rendering
					g_pd3dDevice->EndScene();

					// Present
					if( g_pd3dDevice->Present( 0,0,0,0 ) == D3DERR_DEVICELOST )
					{
						g_Sprite->OnLostDevice();
						g_Font->OnLostDevice();

						if( g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET )
						{
							D3DPRESENT_PARAMETERS d3dpp; 
							ZeroMemory( &d3dpp, sizeof(d3dpp) );
#ifdef DEBUG
							d3dpp.Windowed = TRUE;
#endif
							d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
							d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
							d3dpp.EnableAutoDepthStencil = TRUE;
							d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
							d3dpp.BackBufferWidth = g_ClientRect.right-g_ClientRect.left;
							d3dpp.BackBufferHeight = g_ClientRect.bottom-g_ClientRect.top;
							d3dpp.MultiSampleType = (D3DMULTISAMPLE_TYPE)g_NumSamples;
						
							if( g_pd3dDevice->Reset(&d3dpp) == D3D_OK )
							{
								g_Sprite->OnResetDevice();
								g_Font->OnResetDevice();

								g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
								g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
								g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
								g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC );
								g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAXANISOTROPY, 8 );

								float dwFog;
								g_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
								g_pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, g_Ambient );
								g_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR );
								dwFog = 0.0f;
								g_pd3dDevice->SetRenderState( D3DRS_FOGSTART, *(DWORD *)(&dwFog) );
								dwFog = 100.f;
								g_pd3dDevice->SetRenderState( D3DRS_FOGEND, *(DWORD *)(&dwFog) );

								Resource_Light *pLight = (Resource_Light *)
									g_Resource.GetResourceByName( "GlobalLight" );
								if( pLight )
								{
									g_pd3dDevice->SetLight( 0, &pLight->Light );
									g_pd3dDevice->LightEnable( 0, TRUE );
								}
							}
						}
					}

					// Update mouse
					g_Mouse.Buttons &= 7;
					g_Mouse.Buttons |= g_Mouse.Buttons << 3;
					g_Mouse.PrevPos = g_Mouse.Position;
				}
			}
		}
	}

	// Perform a cleanup of the resources used.
	Cleanup();

	// Exit process.
	return EXIT_SUCCESS;
}

LRESULT __stdcall WndProc( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( g_pContext ) g_pContext->MsgProc(hWnd,uiMsg,wParam,lParam);

	switch(uiMsg) 
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_LBUTTONDOWN:
		g_Mouse.Buttons |= 4;
		break;
	case WM_MBUTTONDOWN:
		g_Mouse.Buttons |= 2;
		break;
	case WM_RBUTTONDOWN:
		g_Mouse.Buttons |= 1;
		break;
	case WM_LBUTTONUP:
		g_Mouse.Buttons &= ~4;
		break;
	case WM_MBUTTONUP:
		g_Mouse.Buttons &= ~2;
		break;
	case WM_RBUTTONUP:
		g_Mouse.Buttons &= ~1;
		break;

	case WM_KILLFOCUS:
		g_Mouse.Clear();
		break;

	case WM_SIZE:
		GetClientRect( g_hWnd, &g_ClientRect );
		GetWindowRect( g_hWnd, &g_WindowRect );
		g_dX = float(g_ClientRect.right-g_ClientRect.left)*0.5f;
		g_dY = float(g_ClientRect.bottom-g_ClientRect.top)*0.5f;
		g_AspectRatio = float(g_ClientRect.right-g_ClientRect.left) /
			float(g_ClientRect.bottom-g_ClientRect.top);
		break;

	case WM_CREATE:
		SetCursor(g_CArrow);
		break;

	case WM_SETCURSOR:
		break;

	default:
		return DefWindowProc(hWnd, uiMsg, wParam, lParam);
	}

	return 0;
}

int Cleanup()
{
	/* Destroy context */
	if( g_pContext ) { g_pContext->Destroy(); g_pContext = nullptr; }

	/* Release resources */
	if( g_Sprite ) g_Sprite->Release();
	if( g_Font ) g_Font->Release();

	if( g_pSound ) g_pSound->Release();

	if( g_pd3dDevice ) g_pd3dDevice->Release();
	if( g_pD3D ) g_pD3D->Release();

	return S_OK;
}

int InitD3D()
{
	/* When a thread calls this method, it requests
	that the Direct3D9 device initalise itself. If the
	machine does not support Direct3D 9, this method
	will exit with a failure code and the process
	will terminate as the game will be unable to run. */

	// Release if components already exist
	if( g_pd3dDevice ) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
	if( g_pD3D ) { g_pD3D->Release(); g_pD3D = nullptr; }

	// Create D3D9 COM Object
	if( !( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	{
		/* This is an error code returned to the calling
		thread if the Direct3DCreate9() API function fails,
		indicating lack of support for Direct3D 9. */
		return E_FAIL;
	}

	// Create device
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
#ifdef DEBUG
	d3dpp.Windowed = TRUE;
#endif
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.BackBufferWidth = g_ClientRect.right-g_ClientRect.left;
	d3dpp.BackBufferHeight = g_ClientRect.bottom-g_ClientRect.top;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;

	if( FAILED( g_pD3D->CreateDevice(
		D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		d3dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;

		if( FAILED( g_pD3D->CreateDevice(
			D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&d3dpp, &g_pd3dDevice ) ) )
		{
			d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;

			if( FAILED( g_pD3D->CreateDevice(
				D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
				D3DCREATE_HARDWARE_VERTEXPROCESSING,
				&d3dpp, &g_pd3dDevice ) ) )
			{
				return E_FAIL;
			}
		}
		else g_NumSamples = 2;
	}
	else g_NumSamples = 4;

	/* Set device rendering states */
	Resource_Light *pLight = new Resource_Light();
	g_Resource.AddResource( pLight, "GlobalLight" );
	pLight->Release();

	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAXANISOTROPY, 8 );

	float fFog;
	g_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, g_Ambient );
	g_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR );
	fFog = 0.0f;
	g_pd3dDevice->SetRenderState( D3DRS_FOGSTART, *(DWORD *)(&fFog) );
	fFog = 100.f;
	g_pd3dDevice->SetRenderState( D3DRS_FOGEND, *(DWORD *)(&fFog) );
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, g_Ambient );
	g_pd3dDevice->LightEnable( 0, TRUE );
	g_pd3dDevice->SetLight( 0, &pLight->Light );
	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

	if( FAILED( D3DXCreateFontA(
		g_pd3dDevice,
		32, 16, 1, 1, 0, 0, 1, 1, 1, "Arial", &g_Font ) ) )
	{
		MessageBoxA( g_hWnd, "Failed to create font.", WindowTitle, MB_ICONHAND );
	}
	if( FAILED( D3DXCreateSprite(
		g_pd3dDevice,
		&g_Sprite ) ) )
	{
		MessageBoxA( g_hWnd, "Failed to create sprite.", WindowTitle, MB_ICONHAND );
	}
	D3DXMATRIX matTransform;
	matTransform._11 = 1.0f; matTransform._21 = 0.0f; matTransform._31 = 0.0f; matTransform._41 = 0.0f;
	matTransform._12 = 0.0f; matTransform._22 = 1.0f; matTransform._32 = 0.0f; matTransform._42 = 0.0f;
	matTransform._13 = 0.0f; matTransform._23 = 0.0f; matTransform._33 = 1.0f; matTransform._43 = 0.0f;
	matTransform._14 = 0.0f; matTransform._24 = 0.0f; matTransform._34 = 0.0f; matTransform._44 = 1.0f;
	g_Sprite->SetTransform( &matTransform );

	/* This is a success code returned to the calling
	thread if the whole Direct3D 9 initialisation
	procedure succeeded. */
	return S_OK;
}

int InitDSound()
{
	if(	FAILED( DirectSoundCreate( NULL, // Use default
		&g_pSound, NULL ) ) )
	{
		return E_FAIL;
	}

	g_pSound->SetCooperativeLevel( g_hWnd, DSSCL_PRIORITY );

	Resource_Sound * rSound = new(std::nothrow) Resource_Sound();
	if( !rSound )
	{
		return E_OUTOFMEMORY;
	}
	g_Resource.AddResource( rSound, "SndLoop" );
	if( FAILED( LoadEmbeddedWAV(
		rSound,
		MAKEINTRESOURCEA(IDR_STR_GuitarLoop) ) ) )
	{
		rSound->Release();
		return E_FAIL;
	}
	rSound->Release();

	rSound = new(std::nothrow) Resource_Sound();
	if( !rSound )
	{
		return E_OUTOFMEMORY;
	}
	g_Resource.AddResource( rSound, "SndHover" );
	if( FAILED( LoadEmbeddedWAV(
		rSound,
		MAKEINTRESOURCEA(IDR_STR_SndHover) ) ) )
	{
		rSound->Release();
		return E_FAIL;
	}
	rSound->Release();

	rSound = new(std::nothrow) Resource_Sound();
	if( !rSound )
	{
		return E_OUTOFMEMORY;
	}
	g_Resource.AddResource( rSound, "SndClick" );
	if( FAILED( LoadEmbeddedWAV(
		rSound,
		MAKEINTRESOURCEA(IDR_STR_SndClick) ) ) )
	{
		rSound->Release();
		return E_FAIL;
	}

	rSound->Release();

	return S_OK;
}







/********************************
	Contexts
********************************/

int GOBJ_CONTEXT_MainMenu::GetObjId()
{
	return GOBJID_CONTEXT_MainMenu;
}
int GOBJ_CONTEXT_MainMenu::Create()
{
	GOBJ_CONTEXT::Create();

	GOBJ_BUTTON *pButton;
	GOBJ_BUTTON_StartGame *&bStartGame = *(GOBJ_BUTTON_StartGame **)&pButton;
	GOBJ_BUTTON_Help *&bHelp = *(GOBJ_BUTTON_Help **)&pButton;
	GOBJ_BUTTON_Options *&bOptions = *(GOBJ_BUTTON_Options **)&pButton;
	GOBJ_BUTTON_Exit *&bExit = *(GOBJ_BUTTON_Exit **)&pButton;
	
	long width = g_ClientRect.right - g_ClientRect.left;
	long height = g_ClientRect.bottom - g_ClientRect.top;

	bStartGame = new GOBJ_BUTTON_StartGame;
	bStartGame->Position.left = (width/2)-128;
	bStartGame->Position.top = 32;
	bStartGame->Position.right = (width/2)+128;
	bStartGame->Position.bottom = 128;
	bStartGame->IsAvailable = true;
	bStartGame->Create();
	this->RegisterObject(bStartGame);

	bHelp = new GOBJ_BUTTON_Help;
	bHelp->Position.left = (width/2)-128;
	bHelp->Position.top = 160;
	bHelp->Position.right = (width/2)+128;
	bHelp->Position.bottom = 256;
	bHelp->IsAvailable = true;
	bHelp->Create();
	this->RegisterObject(bHelp);

	bOptions = new GOBJ_BUTTON_Options;
	bOptions->Position.left = (width/2)-128;
	bOptions->Position.top = 288;
	bOptions->Position.right = (width/2)+128;
	bOptions->Position.bottom = 384;
	bOptions->IsAvailable = true;
	bOptions->Create();
	this->RegisterObject(bOptions);

	bExit = new GOBJ_BUTTON_Exit;
	bExit->Position.left = (width/2)-128;
	bExit->Position.top = 416;
	bExit->Position.right = (width/2)+128;
	bExit->Position.bottom = 512;
	bExit->IsAvailable = true;
	bExit->Create();
	this->RegisterObject(bExit);

	return S_OK;
}
int GOBJ_CONTEXT_MainMenu::MsgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uiMsg )
	{
	case WM_KEYDOWN:
		switch( wParam )
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;
	}

	GOBJ_CONTEXT::MsgProc(hWnd, uiMsg, wParam, lParam);

	return S_OK;
}
int GOBJ_CONTEXT_MainMenu::Render()
{
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, g_Ambient, 1.0f, 0 );

	GOBJ_CONTEXT::Render();

#ifdef DEBUG
	g_Sprite->Begin( D3DXSPRITE_ALPHABLEND );
	g_Font->DrawTextA( g_Sprite, MOWVE_IT_VERSION, -1, &g_ClientRect, 0, 0xffffffff );
	g_Sprite->End();
#endif

	return S_OK;
}

int GOBJ_CONTEXT_HelpScreen::GetObjId()
{
	return GOBJID_CONTEXT_HelpScreen;
}
int GOBJ_CONTEXT_HelpScreen::Initialise()
{
	GOBJ_CONTEXT::Initialise();

	this->page = 0;

	return S_OK;
}
int GOBJ_CONTEXT_HelpScreen::MsgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uiMsg )
	{
	case WM_KEYDOWN:
		switch( wParam )
		{
		case VK_ESCAPE:
			g_Queue.AddRequest( CreateMainMenu );
			break;
		case VK_LEFT:
			if( this->page > 0 ) this->page --;
			break;
		case VK_RIGHT:
			if( this->page < 6 ) this->page ++;
			break;
		}
		break;
	}

	GOBJ_CONTEXT::MsgProc(hWnd, uiMsg, wParam, lParam);

	return S_OK;
}
int GOBJ_CONTEXT_HelpScreen::Render()
{
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffff8888, 1.0f, 0 );

	RECT TextRegion = {
		g_ClientRect.left+16,
		g_ClientRect.top+16,
		g_ClientRect.right,
		g_ClientRect.bottom
	};

	g_Sprite->Begin( D3DXSPRITE_ALPHABLEND );

	// Help introduction
	g_Font->DrawTextA( g_Sprite,
		"Welcome to Mowve It.\n"
		"Press ESC to return to the Main Menu.\n"
		"Use the ARROW keys < > to navigate pages."
		,-1, &TextRegion, 0, 0xff000000 );

	// Pages
	TextRegion.top = 192;
	switch( this->page )
	{
	case 0:
		g_Font->DrawTextA( g_Sprite,
			"This game is about lawn mowing. Enter the game using the\n"
			"'New Game' button.\n"
			"The game begins with a small lawn, at the centre of which\n"
			"resides a lawn mower.\n"
			"This is the 'Mower Mover'.\n"
			"Use WASD or the ARROW keys to move the lawn mower and cut\n"
			"the grass.\n\n"
			"(1/7, goto next page)"
			,-1, &TextRegion, 0, 0xff000000 );
		break;
	case 1:
		g_Font->DrawTextA( g_Sprite,
			"Numbers '1', '2' and '3' allow you to switch\n"
			"between mowers.\n"
			"'1' toggles Mower Mini. This is the fastest of all of the\n"
			"mowers but cuts little grass. It is also relatively\n"
			"difficult to control.\n"
			"'2' toggles Mower Mover.\n"
			"'3' toggles Mower Monster. This is a large mower and cuts\n"
			"the most grass. Mower Monster is the slowest of all mowers.\n\n"
			"(2/7, goto next page)"
			,-1, &TextRegion, 0, 0xff000000 );
		break;
	case 2:
		g_Font->DrawTextA( g_Sprite,
			"The SPACE bar allows you to alternate between two\n"
			"viewing angles. Select whichever is best for your\n"
			"gameplay.\n\n"
			"(3/7, goto next page)"
			,-1, &TextRegion, 0, 0xff000000 );
		break;
	case 3:
		g_Font->DrawTextA( g_Sprite,
			"Your objective is to mow the lawn. All the grass must be cut\n"
			"before proceeding to the next level.\n\n"
			"(4/7, goto next page)"
			,-1, &TextRegion, 0, 0xff000000 );
		break;
	case 4:
		g_Font->DrawTextA( g_Sprite,
			"Collect the gnomes and avoid the ornaments. Gnomes award points\n"
			"whereas the ornaments (time dials) deduct points.\n\n"
			"Do not hit more than two ornaments, else you will\n"
			"lose the game.\n\n"
			"(5/7, goto next page)"
			,-1, &TextRegion, 0, 0xff000000 );
		break;
	case 5:
		g_Font->DrawTextA( g_Sprite,
			"Occasionally Mole Hills will appear. These slow the mower\n"
			"but award points and extra time if completely flattened.\n\n"
			"(6/7, goto next page)"
			,-1, &TextRegion, 0, 0xff000000 );
		break;
	case 6:
		g_Font->DrawTextA( g_Sprite,
			"If all the grass is not cut within the set time, you will lose\n"
			"the game.\n\n"
			"Good luck."
			,-1, &TextRegion, 0, 0xff000000 );
		break;
	}

	g_Sprite->End();

	return S_OK;
}

int GOBJ_CONTEXT_OptionsMenu::GetObjId()
{
	return GOBJID_CONTEXT_OptionsMenu;
}
int GOBJ_CONTEXT_OptionsMenu::Create()
{
	GOBJ_CONTEXT::Create();

	GOBJ_SLIDER *pSlider;
	GOBJ_SLIDER_MusicVolume *&sMusicVolume = *(GOBJ_SLIDER_MusicVolume **)&pSlider;
	GOBJ_SLIDER_GrassDensity *&sGrassDensity = *(GOBJ_SLIDER_GrassDensity **)&pSlider;

	RECT rctText = {32,192,32,192};
	g_Font->DrawTextA( 0, "Music volume (dB): ", -1, &rctText, DT_CALCRECT, 0 );

	sMusicVolume = new(std::nothrow) GOBJ_SLIDER_MusicVolume;
	if( sMusicVolume )
	{
		sMusicVolume->Initialise();
		sMusicVolume->Create();
		sMusicVolume->Position.left = rctText.right+32;
		sMusicVolume->Position.top = 192;
		sMusicVolume->Position.right = rctText.right+160;
		sMusicVolume->Position.bottom = 224;
		this->RegisterObject( sMusicVolume );
	}

	rctText.top = 256;
	rctText.bottom = 256;
	g_Font->DrawTextA( 0, "Grass density (lowest - highest): ", -1, &rctText, DT_CALCRECT, 0 );

	sGrassDensity = new(std::nothrow) GOBJ_SLIDER_GrassDensity;
	if( sGrassDensity )
	{
		sGrassDensity->Initialise();
		sGrassDensity->Create();
		sGrassDensity->Position.left = rctText.right+32;
		sGrassDensity->Position.top = 256;
		sGrassDensity->Position.right = rctText.right+160;
		sGrassDensity->Position.bottom = 288;
		this->RegisterObject( sGrassDensity );
	}

	return S_OK;
}
int GOBJ_CONTEXT_OptionsMenu::MsgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uiMsg )
	{
	case WM_KEYDOWN:
		switch( wParam )
		{
		case VK_ESCAPE:
			g_Queue.AddRequest( CreateMainMenu );
			break;
		}
		break;
	}

	GOBJ_CONTEXT::MsgProc(hWnd, uiMsg, wParam, lParam);

	return S_OK;
}
int GOBJ_CONTEXT_OptionsMenu::Render()
{
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x88ffff, 1.0f, 0 );

	GOBJ_CONTEXT::Render();

	g_Sprite->Begin( D3DXSPRITE_ALPHABLEND );

	RECT rctText = {64,64,64,64};
	g_Font->DrawTextA( g_Sprite,
		"Options:\n(Press ESC to return to Main Menu.)",
		-1, &rctText, DT_CALCRECT, 0xff770000 );
	g_Font->DrawTextA( g_Sprite,
		"Options:\n(Press ESC to return to Main Menu.)",
		-1, &rctText, 0, 0xff770000 );

	rctText.left = 32;
	rctText.right = 32;
	rctText.top = 192;
	rctText.bottom = 192;
	g_Font->DrawTextA( g_Sprite,
		"Music volume (dB): ",
		-1, &rctText, DT_CALCRECT, 0xff775500 );
	g_Font->DrawTextA( g_Sprite,
		"Music volume (dB): ",
		-1, &rctText, 0, 0xff770000 );

	rctText.top = 256;
	rctText.bottom = 256;
	g_Font->DrawTextA( g_Sprite,
		"Grass density (lowest - highest): ",
		-1, &rctText, DT_CALCRECT, 0xff775500 );
	g_Font->DrawTextA( g_Sprite,
		"Grass density (lowest - highest): ",
		-1, &rctText, 0, 0xff770000 );

	g_Sprite->End();

	return S_OK;
}


int GOBJ_CONTEXT_PauseMenu::GetObjId()
{
	return GOBJID_CONTEXT_PauseMenu;
}
int GOBJ_CONTEXT_PauseMenu::Destroy()
{
	if( this->pPrevious ) this->pPrevious->Destroy();

	GOBJ_CONTEXT::Destroy();

	return S_OK;
}
int GOBJ_CONTEXT_PauseMenu::Render()
{
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff221100, 1.0f, 0 );

	GOBJ_CONTEXT::Render();

	RECT DrawRegion = {
		g_ClientRect.left,
		g_ClientRect.top+128,
		g_ClientRect.right,
		g_ClientRect.bottom
	};

	g_Sprite->Begin( D3DXSPRITE_ALPHABLEND );
	g_Font->DrawTextA( g_Sprite,
		"Paused\n\nPress ESC to exit.\nPress SPACE to continue.",
		-1, &DrawRegion, DT_CENTER, 0xffff8800 );
	g_Sprite->End();

	return S_OK;
}
int GOBJ_CONTEXT_PauseMenu::MsgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uiMsg )
	{
	case WM_KEYDOWN:
		switch( wParam )
		{
		case VK_ESCAPE:
			g_Queue.AddRequest( CreateMainMenu );
			break;
		case VK_SPACE:
			g_Queue.AddRequest( PauseMenuRestorePrevious );
			break;
		}
		break;
	}

	GOBJ_CONTEXT::MsgProc(hWnd, uiMsg, wParam, lParam);

	return S_OK;
}

int GOBJ_CONTEXT_MainGame::GetObjId()
{
	return GOBJID_CONTEXT_MainGame;
}
int GOBJ_CONTEXT_MainGame::Create()
{
	this->fGrassCut = 0.0f;
	this->score = 0;
	this->TileWidth = 0;
	this->TileHeight = 0;
	this->OnLevelCompletion = 0;
	this->dwTimer = 0;

	return S_OK;
}
int GOBJ_CONTEXT_MainGame::MsgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uiMsg )
	{
	case WM_KEYDOWN:
		switch( wParam )
		{
		case VK_ESCAPE:
			g_Queue.AddRequest( CreatePauseMenu );
			break;
		case VK_SPACE:
			if( g_CamFollow ) {
				g_CamFollow = false;
				g_Camera.BeginAnimate( (1.f/60.f) );
				g_Camera.SetPosition( &this->vecFarView );
				g_Camera.SetFocus( 0.0f, 0.0f, 0.0f );
			} else {
				g_CamFollow = true;
				g_Camera.BeginAnimate( (1.f/60.f) );
			}
			break;
		}
		break;
	}

	GOBJ_CONTEXT::MsgProc(hWnd, uiMsg, wParam, lParam);

	return S_OK;
}
int GOBJ_CONTEXT_MainGame::Update()
{
	GOBJ_GAME_MOWER *pMower = nullptr;
	float fAccel, fSpawnExtents;

	GOBJ_CONTEXT::Update();

	if( this->dwTimer < 600 )
		goto l_nospawn;

	for( DWORD i = 0; i < this->ListSize; i++ )
	{
		if( this->ObjectList[i] )
		{
			if( this->ObjectList[i]->GetObjId() == GOBJID_GAME_MowerMini ||
				this->ObjectList[i]->GetObjId() == GOBJID_GAME_MowerMover ||
				this->ObjectList[i]->GetObjId() == GOBJID_GAME_MowerMonster )
			{
				pMower = (GOBJ_GAME_MOWER *)this->ObjectList[i];
				break;
			}
		}
	}
	fAccel = pMower->GetAxialAcceleration();
	fSpawnExtents = pMower->GetAxialExtents() * 2.0f;

	if( rand() % 64 == 0 )
	{
		switch( rand() % 3 )
		{
		case 0:
			{
				GOBJ_GAME_Gnome *pGnome = new(std::nothrow) GOBJ_GAME_Gnome;
				if( pGnome )
				{
					pGnome->Initialise();
					pGnome->Create();
					this->RegisterObject( pGnome );
					for( WORD i = 0; i < 128; i++ )
					{
						pGnome->Position[0] = -float(this->TileWidth<<1) + float( rand() % (this->TileWidth<<2) );
						pGnome->Position[2] = -float(this->TileWidth<<1) + float( rand() % (this->TileWidth<<2) );
						if( pMower )
						{
							if( pGnome->Position[0] >= pMower->Position[0]-fSpawnExtents &&
								pGnome->Position[0] <= pMower->Position[0]+fSpawnExtents &&
								pGnome->Position[2] >= pMower->Position[2]-fSpawnExtents &&
								pGnome->Position[2] <= pMower->Position[2]+fSpawnExtents )
							{
								continue;
							}
							else break;
						}
					}
				}
			}
			break;
		case 1:
			{
				GOBJ_GAME_StoneOrnament *pOrnament = new(std::nothrow) GOBJ_GAME_StoneOrnament;
				if( pOrnament )
				{
					pOrnament->Initialise();
					pOrnament->Create();
					this->RegisterObject( pOrnament );
					for( WORD i = 0; i < 128; i++ )
					{
						pOrnament->Position[0] = -float(this->TileWidth<<1) + float( rand() % (this->TileWidth<<2) );
						pOrnament->Position[2] = -float(this->TileWidth<<1) + float( rand() % (this->TileWidth<<2) );
						if( pMower )
						{
							if( pOrnament->Position[0] >= pMower->Position[0]-fSpawnExtents &&
								pOrnament->Position[0] <= pMower->Position[0]+fSpawnExtents &&
								pOrnament->Position[2] >= pMower->Position[2]-fSpawnExtents &&
								pOrnament->Position[2] <= pMower->Position[2]+fSpawnExtents )
							{
								continue;
							}
							else break;
						}
					}
				}
			}
			break;
		default:
			{
				GOBJ_GAME_MoleHill *pMoleHill = new(std::nothrow) GOBJ_GAME_MoleHill;
				if( pMoleHill )
				{
					pMoleHill->Initialise();
					pMoleHill->Create();
					this->RegisterObject( pMoleHill );
					for( WORD i = 0; i < 128; i++ )
					{
						pMoleHill->Position[0] = -float(this->TileWidth<<1) + float( rand() % (this->TileWidth<<2) );
						pMoleHill->Position[2] = -float(this->TileWidth<<1) + float( rand() % (this->TileWidth<<2) );
						if( pMower )
						{
							if( pMoleHill->Position[0] >= pMower->Position[0]-fSpawnExtents &&
								pMoleHill->Position[0] <= pMower->Position[0]+fSpawnExtents &&
								pMoleHill->Position[2] >= pMower->Position[2]-fSpawnExtents &&
								pMoleHill->Position[2] <= pMower->Position[2]+fSpawnExtents )
							{
								continue;
							}
							else break;
						}
					}
				}
			}
			break;
		}
	}

l_nospawn:

	DWORD numGrassTiles = 0, numCutGrassTiles = 0;
	for( DWORD i = 0; i < this->ListSize; i++ )
	{
		if( this->ObjectList[i] )
		{
			if( this->ObjectList[i]->GetObjId() == GOBJID_GAME_GrassTile )
			{
				numGrassTiles ++;
				if( ((GOBJ_GAME_GrassTile*)this->ObjectList[i])->IsMowed )
					numCutGrassTiles ++;
			}
		}
	}

	if( numGrassTiles == numCutGrassTiles )
		this->Congratulations();
	else
		this->fGrassCut = (float(numCutGrassTiles)*100.f) / float(numGrassTiles);

	this->dwTimer --;
	if( this->dwTimer == 0 )
		this->TimeoutGameover();

	return S_OK;
}
int GOBJ_CONTEXT_MainGame::Render()
{
	g_pd3dDevice->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, g_Ambient, 1.0f, 0 );

	D3DXMATRIX matTransform;

	g_Camera.BuildViewMatrix( &matTransform );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matTransform );

	D3DXMatrixPerspectiveFovLH( &matTransform, 1.0f, g_AspectRatio, 1.0f, 100.0f );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matTransform );

	GOBJ_CONTEXT::Render();

	char str[512];
	char num[16];
	strcpy(str,"Grass cut: ");
	_ltoa_s( long(this->fGrassCut),
		num, 16, 10 );
	strcat_s(str,512,num);
	strcat_s(str,512,"%\nScore: ");
	_ltoa_s( this->score,
		num, 16, 10 );
	strcat_s(str,512,num);
	strcat_s(str,512,"\n\nLives remaining: ");
	_ltoa_s( this->dwLives,
		num, 16, 10 );
	strcat_s(str,512,num);
	strcat_s(str,512,"\n\nTime remaining (seconds): ");
	_ltoa_s( this->dwTimer/60L,
		num, 16, 10 );
	strcat_s(str,512,num);
	g_Sprite->Begin( D3DXSPRITE_ALPHABLEND );
	g_Font->DrawTextA( g_Sprite,
		str, -1, &g_ClientRect,
		0, 0xffffffff );
	g_Sprite->End();

	return S_OK;
}



/********************************
	Game objects
********************************/

int GOBJ_BUTTON::Initialise()
{
	this->Position = RECT();
	this->pFace = 0;
	this->IsAvailable = true;

	return S_OK;
}
int GOBJ_BUTTON::Create()
{
	HMODULE hModule = GetModuleHandle(0);
	HRSRC hResInfo;
	HGLOBAL hRes;

	Resource_Texture *pTexture;

	pTexture = (Resource_Texture *)
		g_Resource.GetResourceByName( "ButtonFaceInactive" );
	if( !pTexture )
	{
		pTexture = new Resource_Texture();
		g_Resource.AddResource( pTexture, "ButtonFaceInactive" );
		
		hResInfo = FindResource( hModule, MAKEINTRESOURCE(IDR_STR_ButtonInactive), "RSRC" );
		if( hResInfo )
		{
			hRes = LoadResource( hModule, hResInfo );

			if( FAILED( D3DXCreateTextureFromFileInMemoryEx(
				g_pd3dDevice,
				hRes,
				SizeofResource( hModule, hResInfo ),
				0, 0, 0, 0,
				D3DFMT_UNKNOWN,
				D3DPOOL_MANAGED,
				D3DX_DEFAULT,
				D3DX_DEFAULT,
				0, 0, 0,
				&pTexture->pTexture ) ) )
			{
				MessageBoxA( g_hWnd, "Failed to create texture.", WindowTitle, MB_ICONHAND );
				this->pFace = pTexture->pTexture = nullptr;
			}
			else
				this->pFace = pTexture->pTexture;
		}

		pTexture->Release();
	}
	else
		this->pFace = pTexture->pTexture;

	pTexture = (Resource_Texture *)
		g_Resource.GetResourceByName( "ButtonFaceActive" );
	if( !pTexture )
	{
		pTexture = new Resource_Texture();
		g_Resource.AddResource( pTexture, "ButtonFaceActive" );
		
		hResInfo = FindResource( hModule, MAKEINTRESOURCE(IDR_STR_ButtonActive), "RSRC" );
		if( hResInfo )
		{
			hRes = LoadResource( hModule, hResInfo );

			if( FAILED( D3DXCreateTextureFromFileInMemoryEx(
				g_pd3dDevice,
				hRes,
				SizeofResource( hModule, hResInfo ),
				0, 0, 0, 0,
				D3DFMT_UNKNOWN,
				D3DPOOL_MANAGED,
				D3DX_DEFAULT,
				D3DX_DEFAULT,
				0, 0, 0,
				&pTexture->pTexture ) ) )
			{
				MessageBoxA( g_hWnd, "Failed to create texture.", WindowTitle, MB_ICONHAND );
			}
		}

		pTexture->Release();
	}

	pTexture = (Resource_Texture *)
		g_Resource.GetResourceByName( "ButtonFacePressed" );
	if( !pTexture )
	{
		pTexture = new Resource_Texture();
		g_Resource.AddResource( pTexture, "ButtonFacePressed" );
		
		hResInfo = FindResource( hModule, MAKEINTRESOURCE(IDR_STR_ButtonPressed), "RSRC" );
		if( hResInfo )
		{
			hRes = LoadResource( hModule, hResInfo );

			if( FAILED( D3DXCreateTextureFromFileInMemoryEx(
				g_pd3dDevice,
				hRes,
				SizeofResource( hModule, hResInfo ),
				0, 0, 0, 0,
				D3DFMT_UNKNOWN,
				D3DPOOL_MANAGED,
				D3DX_DEFAULT,
				D3DX_DEFAULT,
				0, 0, 0,
				&pTexture->pTexture ) ) )
			{
				MessageBoxA( g_hWnd, "Failed to create texture.", WindowTitle, MB_ICONHAND );
			}
		}

		pTexture->Release();
	}

	pTexture = (Resource_Texture *)
		g_Resource.GetResourceByName( "ButtonFaceDisabled" );
	if( !pTexture )
	{
		pTexture = new Resource_Texture();
		g_Resource.AddResource( pTexture, "ButtonFaceDisabled" );
		
		hResInfo = FindResource( hModule, MAKEINTRESOURCE(IDR_STR_ButtonDisabled), "RSRC" );
		if( hResInfo )
		{
			hRes = LoadResource( hModule, hResInfo );

			if( FAILED( D3DXCreateTextureFromFileInMemoryEx(
				g_pd3dDevice,
				hRes,
				SizeofResource( hModule, hResInfo ),
				0, 0, 0, 0,
				D3DFMT_UNKNOWN,
				D3DPOOL_MANAGED,
				D3DX_DEFAULT,
				D3DX_DEFAULT,
				0, 0, 0,
				&pTexture->pTexture ) ) )
			{
				MessageBoxA( g_hWnd, "Failed to create texture.", WindowTitle, MB_ICONHAND );
			}
		}

		pTexture->Release();
	}

	return S_OK;
}
int GOBJ_BUTTON::Update()
{
	if( this->IsAvailable )
	{
		if( g_Mouse.Position.x >= this->Position.left &&
			g_Mouse.Position.x <= this->Position.right &&
			g_Mouse.Position.y >= this->Position.top &&
			g_Mouse.Position.y <= this->Position.bottom )
		{
			if( g_Mouse.Buttons & 4 )
			{
				if( !(g_Mouse.Buttons & 32) )
				{
					// Button is pressed
					SetCursor( g_CArrow );
					Resource_Texture *pButtonFace = (Resource_Texture *)
						g_Resource.GetResourceByName( "ButtonFacePressed" );
					this->pFace = pButtonFace->pTexture;
				}
			}
			else if( g_Mouse.PrevPos.x < this->Position.left ||
				g_Mouse.PrevPos.x > this->Position.right ||
				g_Mouse.PrevPos.y < this->Position.top ||
				g_Mouse.PrevPos.y > this->Position.bottom )
			{
				// Cursor has entered
				SetCursor( g_CSelect );
				Resource_Sound *pHover = (Resource_Sound *)
					g_Resource.GetResourceByName( "SndHover" );
				pHover->pBuffer->SetCurrentPosition(0);
				pHover->pBuffer->Play(0,0,0);
				Resource_Texture *pButtonFace = (Resource_Texture *)
					g_Resource.GetResourceByName( "ButtonFaceActive" );
				this->pFace = pButtonFace->pTexture;
			}
		}
		else
		{
			if( g_Mouse.PrevPos.x >= this->Position.left &&
				g_Mouse.PrevPos.x <= this->Position.right &&
				g_Mouse.PrevPos.y >= this->Position.top &&
				g_Mouse.PrevPos.y <= this->Position.bottom )
			{
				// Cursor has left
				SetCursor( g_CArrow );
				Resource_Texture *pButtonFace = (Resource_Texture *)
					g_Resource.GetResourceByName( "ButtonFaceInactive" );
				this->pFace = pButtonFace->pTexture;
			}
		}
	}
	else
	{
		// Cursor has left
		Resource_Texture *pButtonFace = (Resource_Texture *)
			g_Resource.GetResourceByName( "ButtonFaceDisabled" );
		if( pButtonFace ) this->pFace = pButtonFace->pTexture;
		else this->pFace = nullptr;
	}

	return S_OK;
}
int GOBJ_BUTTON::Render()
{
	// Set transforms
	D3DXMATRIX matIdentity;
	matIdentity._11 = 1.0f; matIdentity._21 = 0.0f; matIdentity._31 = 0.0f; matIdentity._41 = 0.0f;
	matIdentity._12 = 0.0f; matIdentity._22 =-1.0f; matIdentity._32 = 0.0f; matIdentity._42 = 0.0f;
	matIdentity._13 = 0.0f; matIdentity._23 = 0.0f; matIdentity._33 = 1.0f; matIdentity._43 = 0.0f;
	matIdentity._14 = 0.0f; matIdentity._24 = 0.0f; matIdentity._34 = 0.0f; matIdentity._44 = 1.0f;
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matIdentity );
	matIdentity._22 = 1.0f;
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matIdentity );
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );
	
	// Set texture
	g_pd3dDevice->SetTexture( 0, this->pFace );

	// Define vertices
	VERTEX verts[4];
	WORD inds[] = { 0,1,2,1,3,2 };
	float RctTrans[] =
	{
		(float(this->Position.left)/g_dX) - 1.0f,
		(float(this->Position.top)/g_dY) - 1.0f,
		(float(this->Position.right)/g_dX) - 1.0f,
		(float(this->Position.bottom)/g_dY) - 1.0f,
	};
	verts[0].Position[0] = RctTrans[0];
	verts[0].Position[1] = RctTrans[1];
	verts[0].Position[2] = 1.0f;
	verts[1].Position[0] = RctTrans[2];
	verts[1].Position[1] = RctTrans[1];
	verts[1].Position[2] = 1.0f;
	verts[2].Position[0] = RctTrans[0];
	verts[2].Position[1] = RctTrans[3];
	verts[2].Position[2] = 1.0f;
	verts[3].Position[0] = RctTrans[2];
	verts[3].Position[1] = RctTrans[3];
	verts[3].Position[2] = 1.0f;

	verts[0].Normal[0] = 0.0f;
	verts[0].Normal[1] = 0.0f;
	verts[0].Normal[2] = 0.0f;
	verts[1].Normal[0] = 0.0f;
	verts[1].Normal[1] = 0.0f;
	verts[1].Normal[2] = 0.0f;
	verts[2].Normal[0] = 0.0f;
	verts[2].Normal[1] = 0.0f;
	verts[2].Normal[2] = 0.0f;
	verts[3].Normal[0] = 0.0f;
	verts[3].Normal[1] = 0.0f;
	verts[3].Normal[2] = 0.0f;

	verts[0].TexCoord[0] = 0.0f;
	verts[0].TexCoord[1] = 0.0f;
	verts[1].TexCoord[0] = 1.0f;
	verts[1].TexCoord[1] = 0.0f;
	verts[2].TexCoord[0] = 0.0f;
	verts[2].TexCoord[1] = 1.0f;
	verts[3].TexCoord[0] = 1.0f;
	verts[3].TexCoord[1] = 1.0f;

	// Set render states
	g_pd3dDevice->SetFVF( D3DFVF_VERTEX );
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	
	// Draw button face
	g_pd3dDevice->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST,
		0, 4, 2, &inds, D3DFMT_INDEX16,
		&verts, sizeof(VERTEX) );

	// Return
	return S_OK;
}

int GOBJ_BUTTON_StartGame::GetObjId()
{
	return GOBJID_GAME_UI_StartGame;
}
int GOBJ_BUTTON_StartGame::Render()
{
	GOBJ_BUTTON::Render();

	// Draw button text
	if( g_Font )
	{
		g_Sprite->Begin( D3DXSPRITE_ALPHABLEND );
		g_Font->DrawTextA( g_Sprite, "New Game", -1, &this->Position, NULL, 0xffffffff );
		g_Sprite->End();
	}

	// Return
	return S_OK;
}
int GOBJ_BUTTON_StartGame::MsgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uiMsg )
	{
	case WM_LBUTTONUP:
		if( this->IsAvailable &&
			g_Mouse.Position.x >= this->Position.left &&
			g_Mouse.Position.x <= this->Position.right &&
			g_Mouse.Position.y >= this->Position.top &&
			g_Mouse.Position.y <= this->Position.bottom )
		{
			Resource_Sound *pClick = (Resource_Sound *)g_Resource.GetResourceByName("SndClick");
			if( pClick )
			{
				pClick->pBuffer->SetCurrentPosition(0);
				pClick->pBuffer->Play(0,0,0);
			}
			g_Queue.AddRequest( CreateLevel1 );
		}
		break;
	}

	return S_OK;
}

int GOBJ_BUTTON_Options::GetObjId()
{
	return GOBJID_GAME_UI_Options;
}
int GOBJ_BUTTON_Options::Render()
{
	GOBJ_BUTTON::Render();

	// Draw button text
	if( g_Font )
	{
		g_Sprite->Begin( D3DXSPRITE_ALPHABLEND );
		g_Font->DrawTextA( g_Sprite, "Options", -1, &this->Position, NULL, 0xffffffff );
		g_Sprite->End();
	}

	// Return
	return S_OK;
}
int GOBJ_BUTTON_Options::MsgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uiMsg )
	{
	case WM_LBUTTONUP:
		if( this->IsAvailable &&
			g_Mouse.Position.x >= this->Position.left &&
			g_Mouse.Position.x <= this->Position.right &&
			g_Mouse.Position.y >= this->Position.top &&
			g_Mouse.Position.y <= this->Position.bottom )
		{
			Resource_Sound *pClick = (Resource_Sound *)g_Resource.GetResourceByName("SndClick");
			if( pClick )
			{
				pClick->pBuffer->SetCurrentPosition(0);
				pClick->pBuffer->Play(0,0,0);
			}
			g_Queue.AddRequest( CreateOptionsMenu );
		}
		break;
	}

	return S_OK;
}

int GOBJ_BUTTON_Exit::GetObjId()
{
	return GOBJID_GAME_UI_Exit;
}
int GOBJ_BUTTON_Exit::Render()
{
	GOBJ_BUTTON::Render();

	// Draw button text
	if( g_Font )
	{
		g_Sprite->Begin( D3DXSPRITE_ALPHABLEND );
		g_Font->DrawTextA( g_Sprite, "Exit", -1, &this->Position, NULL, 0xffffffff );
		g_Sprite->End();
	}

	// Return
	return S_OK;
}
int GOBJ_BUTTON_Exit::MsgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uiMsg )
	{
	case WM_LBUTTONUP:
		if( this->IsAvailable &&
			g_Mouse.Position.x >= this->Position.left &&
			g_Mouse.Position.x <= this->Position.right &&
			g_Mouse.Position.y >= this->Position.top &&
			g_Mouse.Position.y <= this->Position.bottom )
		{
			Resource_Sound *pClick = (Resource_Sound *)g_Resource.GetResourceByName("SndClick");
			if( pClick )
			{
				pClick->pBuffer->SetCurrentPosition(0);
				pClick->pBuffer->Play(0,0,0);
			}
			DestroyWindow(g_hWnd);
		}
		break;
	}

	return S_OK;
}

int GOBJ_BUTTON_Help::GetObjId()
{
	return GOBJID_GAME_UI_Exit;
}
int GOBJ_BUTTON_Help::Render()
{
	GOBJ_BUTTON::Render();

	// Draw button text
	if( g_Font )
	{
		g_Sprite->Begin( D3DXSPRITE_ALPHABLEND );
		g_Font->DrawTextA( g_Sprite, "How To Play", -1, &this->Position, NULL, 0xffffffff );
		g_Sprite->End();
	}

	// Return
	return S_OK;
}
int GOBJ_BUTTON_Help::MsgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uiMsg )
	{
	case WM_LBUTTONUP:
		if( g_Mouse.Position.x >= this->Position.left &&
			g_Mouse.Position.x <= this->Position.right &&
			g_Mouse.Position.y >= this->Position.top &&
			g_Mouse.Position.y <= this->Position.bottom )
		{
			Resource_Sound *pClick = (Resource_Sound *)g_Resource.GetResourceByName("SndClick");
			if( pClick )
			{
				pClick->pBuffer->SetCurrentPosition(0);
				pClick->pBuffer->Play(0,0,0);
			}
			g_Queue.AddRequest( CreateHelpScreen );
		}
		break;
	}

	return S_OK;
}



int GOBJ_SLIDER::Initialise()
{
	this->BackColour = 0xff0000;
	this->FrontColour = 0xff8888;
	this->Position.left = 0;
	this->Position.top = 0;
	this->Position.right = 0;
	this->Position.left = 0;
	this->fSetting = 1.0f;
	this->IsDragged = false;

	return S_OK;
}
int GOBJ_SLIDER::MsgProc( HWND, UINT uiMsg, WPARAM, LPARAM )
{
	switch( uiMsg )
	{
	case WM_LBUTTONDOWN:
		if( g_Mouse.Position.x >= this->Position.left &&
			g_Mouse.Position.x <= this->Position.right &&
			g_Mouse.Position.y >= this->Position.top &&
			g_Mouse.Position.y <= this->Position.bottom )
		{
			this->IsDragged = true;
		}
		break;
	case WM_LBUTTONUP:
		if( this->IsDragged )
		{
			this->IsDragged = false;
			this->OnDrag();
			this->OnRelease();
		}
		break;
	}

	return S_OK;
}
int GOBJ_SLIDER::Update()
{
	if( this->IsDragged )
	{
		if( g_Mouse.Position.x < this->Position.left )
			this->fSetting = 0.0f;
		else if( g_Mouse.Position.x > this->Position.right )
			this->fSetting = 1.0f;
		else
		{
			this->fSetting = float(g_Mouse.Position.x-this->Position.left) /
				float(this->Position.right-this->Position.left);
		}
		this->OnDrag();
	}

	return S_OK;
}
int GOBJ_SLIDER::Render()
{
	RECT rctFill;
	{
		float fDelta = float(this->Position.bottom-this->Position.top)/3.0f;
		rctFill.top = this->Position.top+long(fDelta);
		rctFill.bottom = this->Position.top+long(fDelta*2.0f);
		rctFill.left = this->Position.left;
		rctFill.right = this->Position.right;
	}

	IDirect3DSurface9 *pBackBuffer;
	g_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	g_pd3dDevice->ColorFill( pBackBuffer, &rctFill, this->BackColour );

	{
		float fSlideWidth = float(
			this->Position.right-this->Position.left );
		float fOffset = float(this->Position.left) +
			(this->fSetting*fSlideWidth*0.9f);
		rctFill.left = long( fOffset );
		rctFill.right = long( fOffset+(fSlideWidth*0.1f) );
		rctFill.top = this->Position.top;
		rctFill.bottom = this->Position.bottom;
	}

	g_pd3dDevice->ColorFill( pBackBuffer, &rctFill, this->FrontColour );
	pBackBuffer->Release();

	return S_OK;
}
int GOBJ_SLIDER::OnDrag()
{
	return S_OK;
}
int GOBJ_SLIDER::OnRelease()
{
	return S_OK;
}

int GOBJ_SLIDER_MusicVolume::GetObjId()
{
	return GOBJID_Null;
}
int GOBJ_SLIDER_MusicVolume::Create()
{
	this->fSetting = g_MusicVolume;

	return S_OK;
}
int GOBJ_SLIDER_MusicVolume::OnDrag()
{
	g_MusicVolume = this->fSetting;
	Resource_Sound *pMusic = (Resource_Sound *)
		g_Resource.GetResourceByName( "SndLoop" );
	if( pMusic )
		pMusic->pBuffer->SetVolume( -10000 + long( this->fSetting * 10000.f ) );

	return S_OK;
}

int GOBJ_SLIDER_GrassDensity::GetObjId()
{
	return GOBJID_Null;
}
int GOBJ_SLIDER_GrassDensity::Create()
{
	switch( g_GrassDensity )
	{
	case IDR_STR_Grass1:
		this->fSetting = 1.0f;
		break;
	case IDR_STR_Grass2:
		this->fSetting = 0.5f;
		break;
	default:
		this->fSetting = 0.0f;
		break;
	}

	return S_OK;
}
int GOBJ_SLIDER_GrassDensity::OnDrag()
{
	if( this->fSetting < (1.0f/3.0f) ) {
		g_GrassDensity = IDR_STR_Grass3;
		this->fSetting = 0.0f;
	} else if( this->fSetting < (2.0f/3.0f) ) {
		g_GrassDensity = IDR_STR_Grass2;
		this->fSetting = 0.5f;
	} else {
		g_GrassDensity = IDR_STR_Grass1;
		this->fSetting = 1.0f;
	}

	return S_OK;
}
int GOBJ_SLIDER_GrassDensity::OnRelease()
{
	Resource_Mesh *pGrass = (Resource_Mesh *)
		g_Resource.GetResourceByName( "Grass" );
	if( pGrass )
	{
		g_Resource.ReleaseResource(pGrass);
		pGrass = new(std::nothrow) Resource_Mesh();
		if( pGrass )
		{
			g_Resource.AddResource( pGrass, "Grass" );
			LoadEmbeddedMesh( pGrass, MAKEINTRESOURCEA(g_GrassDensity) );
			pGrass->Release();
		}
	}

	return S_OK;
}



int GOBJ_FloatingText::GetObjId()
{
	return GOBJID_Null;
}
int GOBJ_FloatingText::Initialise()
{
	this->dwColour = 0;
	this->dwInitFrameCount = 60;
	this->dwFrameCount = 60;
	this->dwAnimStage = 0;
	this->pFont = nullptr;
	this->TextString = "...";
	this->Position[0] = 0.0f;
	this->Position[1] = 0.0f;
	this->Position[2] = 0.0f;
	this->Velocity[0] = 0.0f;
	this->Velocity[1] = 0.0f;
	this->Velocity[2] = 0.0f;

	return S_OK;
}
int GOBJ_FloatingText::Create()
{

	return S_OK;
}
int GOBJ_FloatingText::Destroy()
{
	delete this;

	return S_OK;
}
int GOBJ_FloatingText::Update()
{
	if( this->dwAnimStage == 0 )
	{
		this->Position[0] += this->Velocity[0] * 0.5f;
		this->Position[1] += this->Velocity[1] * 0.5f;
		this->Position[2] += this->Velocity[2] * 0.5f;
	}
	else
	{
		this->Position[0] += this->Velocity[0];
		this->Position[1] += this->Velocity[1];
		this->Position[2] += this->Velocity[2];
	}
	this->dwFrameCount --;
	if( this->dwFrameCount <= 0 )
	{
		this->dwAnimStage ++;
		if( this->dwAnimStage > 1 )
		{
			g_pContext->UnregisterObject( this );
			this->Destroy();
		}
		else this->dwFrameCount = this->dwInitFrameCount;
	}

	return S_OK;
}
int GOBJ_FloatingText::Render()
{
	RECT rct =
	{
		long(this->Position[0]),
		long(this->Position[1]),
		g_ClientRect.right,
		g_ClientRect.bottom
	};
	DWORD colour; BYTE alpha;
	if( this->dwAnimStage == 0 )
	{
		float fProgression =
			float( this->dwInitFrameCount - this->dwFrameCount ) /
			float( this->dwInitFrameCount );
		alpha = BYTE( fProgression * 255.0f );
		colour = this->dwColour & 0xffffff;
		colour |= alpha << 24;
	}
	else
	{
		float fProgression =
			float( this->dwInitFrameCount - this->dwFrameCount ) /
			float( this->dwInitFrameCount );
		alpha = BYTE( (1.0f-fProgression) * 255.0f );
		colour = this->dwColour & 0xffffff;
		colour |= alpha << 24;
	}

	this->pFont->DrawTextA( 0,
		this->TextString.GetString(),
		-1, &rct, 0, colour );

	return S_OK;
}



int GOBJ_GAME_GrassTile::GetObjId()
{
	return GOBJID_GAME_GrassTile;
}
int GOBJ_GAME_GrassTile::Initialise()
{
	this->pMesh = nullptr;
	this->Position[0] = 0.0f;
	this->Position[1] = 0.0f;
	this->Position[2] = 0.0f;
	this->IsMowed = false;

	return S_OK;
}
int GOBJ_GAME_GrassTile::Create()
{
	// Has 'Grass.x' already been loaded?
	if( this->pMesh = (Resource_Mesh *)
		g_Resource.GetResourceByName( "Grass" ) )
	{
		this->pMesh->AddRef();
		return S_OK;
	}
	else
	{
		// Allocate
		this->pMesh = new(std::nothrow) Resource_Mesh();
		if( !this->pMesh ) return E_OUTOFMEMORY;
		g_Resource.AddResource( this->pMesh, "Grass" );

		LoadEmbeddedMesh( this->pMesh, MAKEINTRESOURCEA(g_GrassDensity) );
	}

	return S_OK;
}
int GOBJ_GAME_GrassTile::Destroy()
{
	if( this->pMesh )
		this->pMesh->Release();

	delete this;

	return S_OK;
}
int GOBJ_GAME_GrassTile::Render()
{
	D3DXMATRIX mat;
	mat.m[0][0] = 1.0f;						mat.m[2][0] = 0.0f; mat.m[3][0] = this->Position[0];
	mat.m[0][1] = 0.0f;						mat.m[2][1] = 0.0f; mat.m[3][1] = this->Position[1];
	mat.m[0][2] = 0.0f; mat.m[1][2] = 0.0f; mat.m[2][2] = 1.0f; mat.m[3][2] = this->Position[2];
	mat.m[0][3] = 0.0f; mat.m[1][3] = 0.0f; mat.m[2][3] = 0.0f; mat.m[3][3] = 1.0f;

	if( this->IsMowed ) {
		mat.m[1][0] = 0.0f;
		mat.m[1][1] = 0.1f;
	} else {
		mat.m[1][0] = cosf( float(GetTickCount())*0.002f )*0.1f;
		mat.m[1][1] = 1.0f;
	}

	g_pd3dDevice->SetTransform( D3DTS_WORLD, &mat );

	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	if( this->pMesh )
		this->pMesh->Draw();

	return S_OK;
}



int GOBJ_GAME_MOWER::Initialise()
{
	this->pMesh = 0;
	this->Position[0] = 0.0f;
	this->Position[1] = 0.0f;
	this->Position[2] = 0.0f;
	this->Facing[0] = 0.0f;
	this->Facing[1] = 0.0f;
	this->Facing[2] = 1.0f;
	this->Velocity[0] = 0.0f;
	this->Velocity[1] = 0.0f;
	this->Velocity[2] = 0.0f;

	return S_OK;
}
int GOBJ_GAME_MOWER::Destroy()
{
	if( this->pMesh )
		this->pMesh->Release();

	GOBJ_GAME::Destroy();

	return S_OK;
}
int GOBJ_GAME_MOWER::Update()
{
	// Apply velocity
	this->Position[0] += this->Velocity[0];
	this->Position[1] += this->Velocity[1];
	this->Position[2] += this->Velocity[2];

	// Ensure mower doesn't exit bounds
	float BoundsX, BoundsY;
	BoundsX = float( (((GOBJ_CONTEXT_MainGame*)g_pContext)->TileWidth<<1)-1 );
	BoundsY = float( (((GOBJ_CONTEXT_MainGame*)g_pContext)->TileHeight<<1)-1 );
	if( this->Position[0] >= BoundsX ) {
		this->Position[0] = BoundsX;
		this->Velocity[0] = 0.0f;
	} else if( this->Position[0] <=-BoundsX ) {
		this->Position[0] =-BoundsX;
		this->Velocity[0] = 0.0f;
	} if( this->Position[2] >= BoundsY ) {
		this->Position[2] = BoundsY;
		this->Velocity[2] = 0.0f;
	} else if( this->Position[2] <=-BoundsY ) {
		this->Position[2] =-BoundsY;
		this->Velocity[2] = 0.0f;
	}

	// Apply acceleration
	{
		float fAccel = this->GetAxialAcceleration();
		if( g_Keyboard.Keys['A'] & 0x80 ||
			g_Keyboard.Keys[VK_LEFT] & 0x80 )
			this->Velocity[0] -= fAccel;
		if( g_Keyboard.Keys['D'] & 0x80 ||
			g_Keyboard.Keys[VK_RIGHT] & 0x80 )
			this->Velocity[0] += fAccel;
		if( g_Keyboard.Keys['W'] & 0x80 ||
			g_Keyboard.Keys[VK_UP] & 0x80 )
			this->Velocity[2] += fAccel;
		if( g_Keyboard.Keys['S'] & 0x80 ||
			g_Keyboard.Keys[VK_DOWN] & 0x80 )
			this->Velocity[2] -= fAccel;
	}

	// Apply friction
	float vecLength = sqrtf(
		(this->Velocity[0] * this->Velocity[0]) +
		(this->Velocity[1] * this->Velocity[1]) +
		(this->Velocity[2] * this->Velocity[2]) );

	if( vecLength > 0.0f )
	{
		float subtractor = 0.01f * (this->Velocity[0] / vecLength);
		if( subtractor < 0.0f ) {
			if( this->Velocity[0] > subtractor && this->Velocity[0] < -subtractor )
				this->Velocity[0] = 0.0f;
			else
				this->Velocity[0] -= subtractor;
		} else {
			if( this->Velocity[0] < subtractor && this->Velocity[0] > -subtractor )
				this->Velocity[0] = 0.0f;
			else
				this->Velocity[0] -= subtractor;
		}

		subtractor = 0.01f * (this->Velocity[1] / vecLength);
		if( subtractor < 0.0f ) {
			if( this->Velocity[1] > subtractor && this->Velocity[1] < -subtractor )
				this->Velocity[1] = 0.0f;
			else
				this->Velocity[1] -= subtractor;
		} else {
			if( this->Velocity[1] < subtractor && this->Velocity[1] > -subtractor )
				this->Velocity[1] = 0.0f;
			else
				this->Velocity[1] -= subtractor;
		}

		subtractor = 0.01f * (this->Velocity[2] / vecLength);
		if( subtractor < 0.0f ) {
			if( this->Velocity[2] > subtractor && this->Velocity[2] < -subtractor )
				this->Velocity[2] = 0.0f;
			else
				this->Velocity[2] -= subtractor;
		} else {
			if( this->Velocity[2] < subtractor && this->Velocity[2] > -subtractor )
				this->Velocity[2] = 0.0f;
			else
				this->Velocity[2] -= subtractor;
		}
	}

	// Determine whether in contact with grass
	{
		float fExtents = this->GetAxialExtents();
		for( DWORD i = 0; i < g_pContext->ListSize; i++ )
		{
			if( !g_pContext->ObjectList[i] ) continue;
			if( g_pContext->ObjectList[i]->GetObjId() == GOBJID_GAME_GrassTile )
			{
				GOBJ_GAME_GrassTile *pGrass = (GOBJ_GAME_GrassTile *)
					g_pContext->ObjectList[i];
				if( !pGrass->IsMowed )
				{
					if( pGrass->Position[0] <= this->Position[0] + fExtents &&
						pGrass->Position[0] >= this->Position[0] - fExtents &&
						pGrass->Position[2] <= this->Position[2] + fExtents &&
						pGrass->Position[2] >= this->Position[2] - fExtents )
					{
						pGrass->IsMowed = true;
						((GOBJ_CONTEXT_MainGame*)g_pContext)->score += 1;
					}
				}
			}
			else if( g_pContext->ObjectList[i]->GetObjId() == GOBJID_GAME_Gnome )
			{
				// Detect collision with gnome
				GOBJ_GAME_Gnome *pGnome = (GOBJ_GAME_Gnome *)
					g_pContext->ObjectList[i];
				if( !pGnome->IsSmashed )
				{
					if( pGnome->Position[0] >= this->Position[0] - fExtents &&
						pGnome->Position[0] <= this->Position[0] + fExtents &&
						pGnome->Position[2] >= this->Position[2] - fExtents &&
						pGnome->Position[2] <= this->Position[2] + fExtents )
					{
						pGnome->IsSmashed = true;
						pGnome->Velocity[0] = this->Velocity[0];
						pGnome->Velocity[1] = 0.5f;
						pGnome->Velocity[2] = this->Velocity[2];
						((GOBJ_CONTEXT_MainGame*)g_pContext)->score += 50;

						GOBJ_FloatingText *pText = new(std::nothrow) GOBJ_FloatingText;
						if( pText )
						{
							pText->pFont = g_Font;
							pText->TextString = "+50 points";
							pText->dwColour = 0xff00ff00;
							pText->dwInitFrameCount = 60;
							pText->dwAnimStage = 0;
							pText->dwFrameCount = 60;
							pText->Velocity[0] = 0.0f;
							pText->Velocity[1] =-1.0f;
							pText->Velocity[2] = 0.0f;
							pText->Position[0] = 32.0f;
							pText->Position[1] = 256.0f;
							pText->Position[2] = 1.0f;
							g_pContext->RegisterObject( pText );
						}
					}
				}
			}
			else if( g_pContext->ObjectList[i]->GetObjId() == GOBJID_GAME_StoneOrnament )
			{
				// Detect collision with ornament
				GOBJ_GAME_StoneOrnament *pOrnament = (GOBJ_GAME_StoneOrnament *)
					g_pContext->ObjectList[i];
				if( !pOrnament->IsSmashed )
				{
					if( pOrnament->Position[0] >= this->Position[0] - fExtents &&
						pOrnament->Position[0] <= this->Position[0] + fExtents &&
						pOrnament->Position[2] >= this->Position[2] - fExtents &&
						pOrnament->Position[2] <= this->Position[2] + fExtents )
					{
						DWORD &_Lives = ((GOBJ_CONTEXT_MainGame*)g_pContext)->dwLives;
						if( _Lives == 0 ) ((GOBJ_CONTEXT_MainGame*)g_pContext)->NoLivesGameover();
						else _Lives --;

						long &_Score = ((GOBJ_CONTEXT_MainGame*)g_pContext)->score;
						_Score -= 100;
						if( _Score < 0 ) _Score = 0;

						pOrnament->IsSmashed = true;
						pOrnament->Velocity[0] = this->Velocity[0];
						pOrnament->Velocity[1] = 0.5f;
						pOrnament->Velocity[2] = this->Velocity[2];

						GOBJ_FloatingText *pText = new(std::nothrow) GOBJ_FloatingText;
						if( pText )
						{
							pText->pFont = g_Font;
							pText->TextString = "-100 points";
							pText->dwColour = 0xffff0000;
							pText->dwInitFrameCount = 60;
							pText->dwAnimStage = 0;
							pText->dwFrameCount = 60;
							pText->Velocity[0] = 0.0f;
							pText->Velocity[1] =-1.0f;
							pText->Velocity[2] = 0.0f;
							pText->Position[0] = 32.0f;
							pText->Position[1] = 256.0f;
							pText->Position[2] = 1.0f;
							g_pContext->RegisterObject( pText );
						}
					}
				}
			}
			else if( g_pContext->ObjectList[i]->GetObjId() == GOBJID_GAME_MoleHill )
			{
				// Detect collision with molehill
				GOBJ_GAME_MoleHill *pMoleHill = (GOBJ_GAME_MoleHill *)
					g_pContext->ObjectList[i];
				if( pMoleHill->Position[0] >= this->Position[0] - fExtents &&
					pMoleHill->Position[0] <= this->Position[0] + fExtents &&
					pMoleHill->Position[2] >= this->Position[2] - fExtents &&
					pMoleHill->Position[2] <= this->Position[2] + fExtents )
				{
					pMoleHill->SquashScale -= 0.021f;
					if( pMoleHill->SquashScale <= 0.001f )
					{
						g_pContext->UnregisterObject( pMoleHill );
						pMoleHill->Destroy();

						long &_Score = ((GOBJ_CONTEXT_MainGame*)g_pContext)->score;
						_Score += 100;
						if( _Score < 0 ) _Score = 0;

						((GOBJ_CONTEXT_MainGame*)g_pContext)->dwTimer += 180;

						GOBJ_FloatingText *pText = new(std::nothrow) GOBJ_FloatingText;
						if( pText )
						{
							pText->pFont = g_Font;
							pText->TextString = "+100 points\n+3 seconds.";
							pText->dwColour = 0xffffff44;
							pText->dwInitFrameCount = 60;
							pText->dwAnimStage = 0;
							pText->dwFrameCount = 60;
							pText->Velocity[0] = 0.0f;
							pText->Velocity[1] =-1.0f;
							pText->Velocity[2] = 0.0f;
							pText->Position[0] = 32.0f;
							pText->Position[1] = 256.0f;
							pText->Position[2] = 1.0f;
							g_pContext->RegisterObject( pText );
						}
					}
					this->Velocity[0] *= 0.8f;
					this->Velocity[2] *= 0.8f;
				}
			}
		}
	}

	if( g_CamFollow )
	{
		g_Camera.SetFocus( (D3DXVECTOR3 *)this->Position );
		g_Camera.SetPosition( this->Position[0], 10.0f, this->Position[2]-20.0f );
	}

	return S_OK;
}
int GOBJ_GAME_MOWER::Render()
{
	D3DXMATRIX mat;
	mat.m[0][0] = 1.0f; mat.m[1][0] = 0.0f; mat.m[2][0] = 0.0f; mat.m[3][0] = this->Position[0];
	mat.m[0][1] = 0.0f; mat.m[1][1] = 1.0f; mat.m[2][1] = 0.0f; mat.m[3][1] = this->Position[1];
	mat.m[0][2] = 0.0f; mat.m[1][2] = 0.0f; mat.m[2][2] = 1.0f; mat.m[3][2] = this->Position[2];
	mat.m[0][3] = 0.0f; mat.m[1][3] = 0.0f; mat.m[2][3] = 0.0f; mat.m[3][3] = 1.0f;
	D3DXMATRIX matRot;

	float vecZero[] = { 0.0f, 0.0f, 0.0f };
	float vecUp[] = { 0.0f, 1.0f, 0.0f };
	if( this->Velocity[0] != 0.0f ||
		this->Velocity[2] != 0.0f )
	{
		float divisor;
		this->Facing[0] = this->Velocity[0];
		this->Facing[1] = this->Velocity[1];
		this->Facing[2] =-this->Velocity[2];
		divisor = sqrtf(
			this->Facing[0] * this->Facing[0] +
			this->Facing[1] * this->Facing[1] +
			this->Facing[2] * this->Facing[2] );
		this->Facing[0] /= divisor;
		this->Facing[1] /= divisor;
		this->Facing[2] /= divisor;
	}
	D3DXMatrixLookAtLH( &matRot,
		(D3DXVECTOR3*)vecZero,
		(D3DXVECTOR3*)this->Facing,
		(D3DXVECTOR3*)vecUp );

	mat = matRot * mat;
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &mat );

	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	if( this->pMesh )
		this->pMesh->Draw();

	return S_OK;
}

int __stdcall SwitchToMowerMini()
{
	GOBJ_GAME_MowerMini *pMini = new(std::nothrow) GOBJ_GAME_MowerMini;
	if( !pMini ) return E_FAIL;

	for( DWORD i = 0; i < g_pContext->ListSize; i++ )
	{
		if( g_pContext->ObjectList[i] )
		{
			if( g_pContext->ObjectList[i]->GetObjId() == GOBJID_GAME_MowerMini )
			{
				delete pMini;
				return S_OK;
			}
			else if(
				g_pContext->ObjectList[i]->GetObjId() == GOBJID_GAME_MowerMover ||
				g_pContext->ObjectList[i]->GetObjId() == GOBJID_GAME_MowerMonster )
			{
				GOBJ_GAME_MOWER &Mower = *(GOBJ_GAME_MOWER *)g_pContext->ObjectList[i];
				float fPos[3], fVel[3];
				fPos[0] = Mower.Position[0];
				fPos[1] = Mower.Position[1];
				fPos[2] = Mower.Position[2];
				fVel[0] = Mower.Velocity[0];
				fVel[1] = Mower.Velocity[1];
				fVel[2] = Mower.Velocity[2];
				Mower.Destroy();
				g_pContext->ObjectList[i] = pMini;
				pMini->Initialise();
				pMini->Create();
				pMini->Position[0] = fPos[0];
				pMini->Position[1] = fPos[1];
				pMini->Position[2] = fPos[2];
				pMini->Velocity[0] = fVel[0];
				pMini->Velocity[1] = fVel[1];
				pMini->Velocity[2] = fVel[2];
				return S_OK;
			}
		}
	}

	return S_OK;
}
int __stdcall SwitchToMowerMover()
{
	GOBJ_GAME_MowerMover *pMover = new(std::nothrow) GOBJ_GAME_MowerMover;
	if( !pMover ) return E_FAIL;

	for( DWORD i = 0; i < g_pContext->ListSize; i++ )
	{
		if( g_pContext->ObjectList[i] )
		{
			if( g_pContext->ObjectList[i]->GetObjId() == GOBJID_GAME_MowerMover )
			{
				delete pMover;
				return S_OK;
			}
			else if(
				g_pContext->ObjectList[i]->GetObjId() == GOBJID_GAME_MowerMini ||
				g_pContext->ObjectList[i]->GetObjId() == GOBJID_GAME_MowerMonster )
			{
				GOBJ_GAME_MOWER &Mower = *(GOBJ_GAME_MOWER *)g_pContext->ObjectList[i];
				float fPos[3], fVel[3];
				fPos[0] = Mower.Position[0];
				fPos[1] = Mower.Position[1];
				fPos[2] = Mower.Position[2];
				fVel[0] = Mower.Velocity[0];
				fVel[1] = Mower.Velocity[1];
				fVel[2] = Mower.Velocity[2];
				Mower.Destroy();
				g_pContext->ObjectList[i] = pMover;
				pMover->Initialise();
				pMover->Create();
				pMover->Position[0] = fPos[0];
				pMover->Position[1] = fPos[1];
				pMover->Position[2] = fPos[2];
				pMover->Velocity[0] = fVel[0];
				pMover->Velocity[1] = fVel[1];
				pMover->Velocity[2] = fVel[2];
				return S_OK;
			}
		}
	}

	return S_OK;
}
int __stdcall SwitchToMowerMonster()
{
	GOBJ_GAME_MowerMonster *pMonster = new(std::nothrow) GOBJ_GAME_MowerMonster;
	if( !pMonster ) return E_FAIL;

	for( DWORD i = 0; i < g_pContext->ListSize; i++ )
	{
		if( g_pContext->ObjectList[i] )
		{
			if( g_pContext->ObjectList[i]->GetObjId() == GOBJID_GAME_MowerMonster )
			{
				delete pMonster;
				return S_OK;
			}
			else if(
				g_pContext->ObjectList[i]->GetObjId() == GOBJID_GAME_MowerMini ||
				g_pContext->ObjectList[i]->GetObjId() == GOBJID_GAME_MowerMover )
			{
				GOBJ_GAME_MOWER &Mower = *(GOBJ_GAME_MOWER *)g_pContext->ObjectList[i];
				float fPos[3], fVel[3];
				fPos[0] = Mower.Position[0];
				fPos[1] = Mower.Position[1];
				fPos[2] = Mower.Position[2];
				fVel[0] = Mower.Velocity[0];
				fVel[1] = Mower.Velocity[1];
				fVel[2] = Mower.Velocity[2];
				Mower.Destroy();
				g_pContext->ObjectList[i] = pMonster;
				pMonster->Initialise();
				pMonster->Create();
				pMonster->Position[0] = fPos[0];
				pMonster->Position[1] = fPos[1];
				pMonster->Position[2] = fPos[2];
				pMonster->Velocity[0] = fVel[0];
				pMonster->Velocity[1] = fVel[1];
				pMonster->Velocity[2] = fVel[2];
				return S_OK;
			}
		}
	}

	return S_OK;
}

int GOBJ_GAME_MowerMini::GetObjId()
{
	return GOBJID_GAME_MowerMini;
}
int GOBJ_GAME_MowerMini::Create()
{
	// Has 'MowerMini.x' already been loaded?
	if( this->pMesh = (Resource_Mesh *)
		g_Resource.GetResourceByName( "MowerMini" ) )
	{
		this->pMesh->AddRef();
		return S_OK;
	}
	else
	{
		// Allocate
		this->pMesh = new(std::nothrow) Resource_Mesh();
		if( !this->pMesh ) return E_OUTOFMEMORY;
		g_Resource.AddResource( this->pMesh, "MowerMini" );

		LoadEmbeddedMesh( this->pMesh, MAKEINTRESOURCEA(IDR_STR_MowerMini) );
	}

	return S_OK;
}
int GOBJ_GAME_MowerMini::MsgProc( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uiMsg )
	{
	case WM_KEYDOWN:
		if( GetKeyState('2') & 0x8000 )
			g_Queue.AddRequest( SwitchToMowerMover );
		else if( GetKeyState('3') & 0x8000 )
			g_Queue.AddRequest( SwitchToMowerMonster );
		break;
	}

	return 0;
}
float GOBJ_GAME_MowerMini::GetAxialExtents()
{
	return 1.5f;
}
float GOBJ_GAME_MowerMini::GetAxialAcceleration()
{
	return 0.02f;
}

int GOBJ_GAME_MowerMover::GetObjId()
{
	return GOBJID_GAME_MowerMover;
}
int GOBJ_GAME_MowerMover::Create()
{
	// Has 'MowerMover.x' already been loaded?
	if( this->pMesh = (Resource_Mesh *)
		g_Resource.GetResourceByName( "MowerMover" ) )
	{
		this->pMesh->AddRef();
		return S_OK;
	}
	else
	{
		// Allocate
		this->pMesh = new(std::nothrow) Resource_Mesh();
		if( !this->pMesh ) return E_OUTOFMEMORY;
		g_Resource.AddResource( this->pMesh, "MowerMover" );

		LoadEmbeddedMesh( this->pMesh, MAKEINTRESOURCEA(IDR_STR_MowerMover) );
	}

	return S_OK;
}
int GOBJ_GAME_MowerMover::MsgProc( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uiMsg )
	{
	case WM_KEYDOWN:
		if( GetKeyState('1') & 0x8000 )
			g_Queue.AddRequest( SwitchToMowerMini );
		else if( GetKeyState('3') & 0x8000 )
			g_Queue.AddRequest( SwitchToMowerMonster );
		break;
	}

	return 0;
}
float GOBJ_GAME_MowerMover::GetAxialExtents()
{
	return 2.0f;
}
float GOBJ_GAME_MowerMover::GetAxialAcceleration()
{
	return 0.015f;
}

int GOBJ_GAME_MowerMonster::GetObjId()
{
	return GOBJID_GAME_MowerMonster;
}
int GOBJ_GAME_MowerMonster::Create()
{
	// Has 'MowerMover.x' already been loaded?
	if( this->pMesh = (Resource_Mesh *)
		g_Resource.GetResourceByName( "MowerMonster" ) )
	{
		this->pMesh->AddRef();
		return S_OK;
	}
	else
	{
		// Allocate
		this->pMesh = new(std::nothrow) Resource_Mesh();
		if( !this->pMesh ) return E_OUTOFMEMORY;
		g_Resource.AddResource( this->pMesh, "MowerMonster" );

		LoadEmbeddedMesh( this->pMesh, MAKEINTRESOURCEA(IDR_STR_MowerMonster) );
	}

	return S_OK;
}
int GOBJ_GAME_MowerMonster::MsgProc( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uiMsg )
	{
	case WM_KEYDOWN:
		if( GetKeyState('1') & 0x8000 )
			g_Queue.AddRequest( SwitchToMowerMini );
		else if( GetKeyState('2') & 0x8000 )
			g_Queue.AddRequest( SwitchToMowerMover );
		break;
	}

	return 0;
}
float GOBJ_GAME_MowerMonster::GetAxialExtents()
{
	return 3.0f;
}
float GOBJ_GAME_MowerMonster::GetAxialAcceleration()
{
	return 0.012f;
}

int GOBJ_GAME_Gnome::GetObjId()
{
	return GOBJID_GAME_Gnome;
}
int GOBJ_GAME_Gnome::Initialise()
{
	this->pMesh = 0;
	this->FrameCount = 240;
	this->Position[0] = 0.0f;
	this->Position[1] = 0.0f;
	this->Position[2] = 0.0f;
	this->Velocity[0] = 0.0f;
	this->Velocity[1] = 0.0f;
	this->Velocity[2] = 0.0f;
	this->IsSmashed = false;

	return S_OK;
}
int GOBJ_GAME_Gnome::Create()
{
	// Has 'Gnome.x' already been loaded?
	if( this->pMesh = (Resource_Mesh *)
		g_Resource.GetResourceByName( "Gnome" ) )
	{
		this->pMesh->AddRef();
		return S_OK;
	}
	else
	{
		// Allocate
		this->pMesh = new(std::nothrow) Resource_Mesh();
		if( !this->pMesh ) return E_OUTOFMEMORY;
		g_Resource.AddResource( this->pMesh, "Gnome" );

		LoadEmbeddedMesh( this->pMesh, MAKEINTRESOURCEA(IDR_STR_Gnome) );
	}

	return S_OK;
}
int GOBJ_GAME_Gnome::Destroy()
{
	if( this->pMesh )
		this->pMesh->Release();

	GOBJ_GAME::Destroy();

	return S_OK;
}
int GOBJ_GAME_Gnome::Update()
{
	if( this->IsSmashed )
	{
		// Apply gravity
		this->Velocity[1] -= 0.03f;

		// Apply velocity
		this->Position[0] += this->Velocity[0];
		this->Position[1] += this->Velocity[1];
		this->Position[2] += this->Velocity[2];

		// Destroy when below certain point
		if( this->Position[1] <= -50.0f )
		{
			g_pContext->UnregisterObject( this );
			this->Destroy();
		}
	}
	else
	{
		this->FrameCount --;
		if( this->FrameCount <= 0 )
		{
			g_pContext->UnregisterObject( this );
			this->Destroy();
		}
	}

	return S_OK;
}
int GOBJ_GAME_Gnome::Render()
{
	D3DXMATRIX mat;
	mat.m[0][0] = 1.0f; mat.m[1][0] = 0.0f; mat.m[2][0] = 0.0f; mat.m[3][0] = this->Position[0];
	mat.m[0][1] = 0.0f; mat.m[1][1] = 1.0f; mat.m[2][1] = 0.0f; mat.m[3][1] = this->Position[1];
	mat.m[0][2] = 0.0f; mat.m[1][2] = 0.0f; mat.m[2][2] = 1.0f; mat.m[3][2] = this->Position[2];
	mat.m[0][3] = 0.0f; mat.m[1][3] = 0.0f; mat.m[2][3] = 0.0f; mat.m[3][3] = 1.0f;

	g_pd3dDevice->SetTransform( D3DTS_WORLD, &mat );

	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	if( this->pMesh )
		this->pMesh->Draw();

	return S_OK;
}

int GOBJ_GAME_StoneOrnament::GetObjId()
{
	return GOBJID_GAME_StoneOrnament;
}
int GOBJ_GAME_StoneOrnament::Initialise()
{
	this->pMesh = 0;
	this->FrameCount = 360;
	this->Position[0] = 0.0f;
	this->Position[1] = 0.0f;
	this->Position[2] = 0.0f;
	this->Velocity[0] = 0.0f;
	this->Velocity[1] = 0.0f;
	this->Velocity[2] = 0.0f;
	this->IsSmashed = false;

	return S_OK;
}
int GOBJ_GAME_StoneOrnament::Create()
{
	// Has 'StoneOrnament.x' already been loaded?
	if( this->pMesh = (Resource_Mesh *)
		g_Resource.GetResourceByName( "Ornament" ) )
	{
		this->pMesh->AddRef();
		return S_OK;
	}
	else
	{
		// Allocate
		this->pMesh = new(std::nothrow) Resource_Mesh();
		if( !this->pMesh ) return E_OUTOFMEMORY;
		g_Resource.AddResource( this->pMesh, "Ornament" );

		LoadEmbeddedMesh( this->pMesh, MAKEINTRESOURCEA(IDR_STR_Ornament) );
	}

	return S_OK;
}
int GOBJ_GAME_StoneOrnament::Destroy()
{
	if( this->pMesh )
		this->pMesh->Release();

	GOBJ_GAME::Destroy();

	return S_OK;
}
int GOBJ_GAME_StoneOrnament::Update()
{
	if( this->IsSmashed )
	{
		// Apply gravity
		this->Velocity[1] -= 0.03f;

		// Apply velocity
		this->Position[0] += this->Velocity[0];
		this->Position[1] += this->Velocity[1];
		this->Position[2] += this->Velocity[2];

		// Destroy when below certain point
		if( this->Position[1] <= -50.0f )
		{
			g_pContext->UnregisterObject( this );
			this->Destroy();
		}
	}
	else
	{
		this->FrameCount --;
		if( this->FrameCount <= 0 )
		{
			g_pContext->UnregisterObject( this );
			this->Destroy();
		}
	}

	return S_OK;
}
int GOBJ_GAME_StoneOrnament::Render()
{
	D3DXMATRIX mat;
	mat.m[0][0] = 1.0f; mat.m[1][0] = 0.0f; mat.m[2][0] = 0.0f; mat.m[3][0] = this->Position[0];
	mat.m[0][1] = 0.0f; mat.m[1][1] = 1.0f; mat.m[2][1] = 0.0f; mat.m[3][1] = this->Position[1];
	mat.m[0][2] = 0.0f; mat.m[1][2] = 0.0f; mat.m[2][2] = 1.0f; mat.m[3][2] = this->Position[2];
	mat.m[0][3] = 0.0f; mat.m[1][3] = 0.0f; mat.m[2][3] = 0.0f; mat.m[3][3] = 1.0f;

	g_pd3dDevice->SetTransform( D3DTS_WORLD, &mat );

	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	if( this->pMesh )
		this->pMesh->Draw();

	return S_OK;
}

int GOBJ_GAME_MoleHill::GetObjId()
{
	return GOBJID_GAME_MoleHill;
}
int GOBJ_GAME_MoleHill::Initialise()
{
	this->pMesh = 0;
	this->FrameCount = 600;
	this->Position[0] = 0.0f;
	this->Position[1] = 0.0f;
	this->Position[2] = 0.0f;
	this->SquashScale = 1.0f;

	return S_OK;
}
int GOBJ_GAME_MoleHill::Create()
{
	// Has 'Molehill.x' already been loaded?
	if( this->pMesh = (Resource_Mesh *)
		g_Resource.GetResourceByName( "MoleHill" ) )
	{
		this->pMesh->AddRef();
		return S_OK;
	}
	else
	{
		// Allocate
		this->pMesh = new(std::nothrow) Resource_Mesh();
		if( !this->pMesh ) return E_OUTOFMEMORY;
		g_Resource.AddResource( this->pMesh, "MoleHill" );

		LoadEmbeddedMesh( this->pMesh, MAKEINTRESOURCEA(IDR_STR_MoleHill) );
	}

	return S_OK;
}
int GOBJ_GAME_MoleHill::Destroy()
{
	if( this->pMesh )
		this->pMesh->Release();

	GOBJ_GAME::Destroy();

	return S_OK;
}
int GOBJ_GAME_MoleHill::Update()
{
	this->SquashScale += 0.001f;
	if( this->SquashScale > 1.0f )
		this->SquashScale = 1.0f;

	return S_OK;
}
int GOBJ_GAME_MoleHill::Render()
{
	D3DXMATRIX mat;
	mat.m[0][0] = 1.0f; mat.m[1][0] = 0.0f; mat.m[2][0] = 0.0f; mat.m[3][0] = this->Position[0];
	mat.m[0][1] = 0.0f;						mat.m[2][1] = 0.0f; mat.m[3][1] = this->Position[1];
	mat.m[0][2] = 0.0f; mat.m[1][2] = 0.0f; mat.m[2][2] = 1.0f; mat.m[3][2] = this->Position[2];
	mat.m[0][3] = 0.0f; mat.m[1][3] = 0.0f; mat.m[2][3] = 0.0f; mat.m[3][3] = 1.0f;

	mat.m[1][1] = this->SquashScale;
	
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &mat );

	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	if( this->pMesh )
		this->pMesh->Draw();

	return S_OK;
}

int GOBJ_GAME_RabbitHelper::GetObjId()
{
	return GOBJID_GAME_RabbitHelper;
}
int GOBJ_GAME_RabbitHelper::Initialise()
{
	this->pMesh = 0;
	this->FrameCount = 600;
	this->Position[0] = 0.0f;
	this->Position[1] = 0.0f;
	this->Position[2] = 0.0f;

	return S_OK;
}
int GOBJ_GAME_RabbitHelper::Create()
{
	// Has 'Molehill.x' already been loaded?
	if( this->pMesh = (Resource_Mesh *)
		g_Resource.GetResourceByName( "Rabbit" ) )
	{
		this->pMesh->AddRef();
		return S_OK;
	}
	else
	{
		// Allocate
		this->pMesh = new(std::nothrow) Resource_Mesh();
		if( !this->pMesh ) return E_OUTOFMEMORY;
		g_Resource.AddResource( this->pMesh, "Rabbit" );

		LoadEmbeddedMesh( this->pMesh, MAKEINTRESOURCEA(IDR_STR_RabbitHelper) );
	}

	return S_OK;
}
int GOBJ_GAME_RabbitHelper::Destroy()
{
	if( this->pMesh )
		this->pMesh->Release();

	GOBJ_GAME::Destroy();

	return S_OK;
}
int GOBJ_GAME_RabbitHelper::Update()
{
	this->FrameCount --;
	if( this->FrameCount == 0 )
	{
		g_pContext->UnregisterObject( this );
		this->Destroy();
	}

	return S_OK;
}
int GOBJ_GAME_RabbitHelper::Render()
{
	D3DXMATRIX mat;
	mat.m[0][0] = 1.0f; mat.m[1][0] = 0.0f; mat.m[2][0] = 0.0f; mat.m[3][0] = this->Position[0];
	mat.m[0][1] = 0.0f; mat.m[1][1] = 1.0f; mat.m[2][1] = 0.0f; mat.m[3][1] = this->Position[1];
	mat.m[0][2] = 0.0f; mat.m[1][2] = 0.0f; mat.m[2][2] = 1.0f; mat.m[3][2] = this->Position[2];
	mat.m[0][3] = 0.0f; mat.m[1][3] = 0.0f; mat.m[2][3] = 0.0f; mat.m[3][3] = 1.0f;
	D3DXMATRIX matRot;

	float vecZero[] = { 0.0f, 0.0f, 0.0f };
	float vecUp[] = { 0.0f, 1.0f, 0.0f };
	if( this->Velocity[0] != 0.0f ||
		this->Velocity[2] != 0.0f )
	{
		float divisor;
		this->Facing[0] = this->Velocity[0];
		this->Facing[1] = this->Velocity[1];
		this->Facing[2] =-this->Velocity[2];
		divisor = sqrtf(
			this->Facing[0] * this->Facing[0] +
			this->Facing[1] * this->Facing[1] +
			this->Facing[2] * this->Facing[2] );
		this->Facing[0] /= divisor;
		this->Facing[1] /= divisor;
		this->Facing[2] /= divisor;
	}
	D3DXMatrixLookAtLH( &matRot,
		(D3DXVECTOR3*)vecZero,
		(D3DXVECTOR3*)this->Facing,
		(D3DXVECTOR3*)vecUp );

	mat = matRot * mat;
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &mat );

	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	if( this->pMesh )
		this->pMesh->Draw();

	return S_OK;
}



/********************************
	Resources
********************************/

Resource_Texture::Resource_Texture() : Resource()
{
	this->pTexture = nullptr;
}
Resource_Texture::~Resource_Texture()
{
	if( this->pTexture ) this->pTexture->Release();
}

Resource_Sprite::Resource_Sprite() : Resource()
{
	this->pTexture = nullptr;
}
Resource_Sprite::~Resource_Sprite()
{
	if( this->pTexture ) this->pTexture->Release();
}
void Resource_Sprite::GetRect(LPRECT pOut)
{
	DWORD dwSpriteIndex = DWORD(this->fImageIndex);
	DWORD dwMaxIndex;
	D3DSURFACE_DESC desc;
	this->pTexture->GetLevelDesc( 0, &desc );
	dwMaxIndex = (desc.Width/this->dwSpriteWidth) - 1;
	while( dwSpriteIndex > dwMaxIndex ) dwSpriteIndex -= dwMaxIndex;
	pOut->left = this->dwSpriteWidth * dwSpriteIndex;
	pOut->right = pOut->left + this->dwSpriteWidth;
	pOut->top = 0;
	pOut->bottom = desc.Height;
}
void Resource_Sprite::Update()
{
	this->fImageIndex += this->fImageSpeed;
}

Resource_Mesh::Resource_Mesh() : Resource()
{
	this->pMesh = nullptr;
	this->ppTextures = nullptr;
}
Resource_Mesh::~Resource_Mesh()
{
	D3DXMESHCONTAINER * MESH = this->pMesh;
	DWORD NumMaterials;
	if( MESH ) NumMaterials = MESH->NumMaterials;
	else NumMaterials = 0;

	while( MESH )
	{
		if( MESH->pSkinInfo ) MESH->pSkinInfo->Release();
		if( MESH->MeshData.pMesh ) MESH->MeshData.pMesh->Release();
		if( MESH->Name ) delete[] MESH->Name;
		if( MESH->pMaterials ) delete[] MESH->pMaterials;
		if( MESH->pEffects ) delete[] MESH->pEffects;
		if( MESH->pAdjacency ) delete[] MESH->pAdjacency;
		D3DXMESHCONTAINER * Next = MESH->pNextMeshContainer;
		delete MESH; MESH = Next;
	}

	if( this->ppTextures )
	{
		for( DWORD i = 0; i < NumMaterials; i++ )
		{
			if( this->ppTextures[i] )
				this->ppTextures[i]->Release();
		}
		delete[] this->ppTextures;
	}
}
int Resource_Mesh::Draw()
{
	if( !this->pMesh ) return S_OK;

	for( DWORD i = 0; i < this->pMesh->NumMaterials; i++ )
	{
		// Set texture and material
		if( this->ppTextures )
		{
			if( this->ppTextures[i] )
				g_pd3dDevice->SetTexture( 0, this->ppTextures[i]->pTexture );
			else
				g_pd3dDevice->SetTexture( 0, nullptr );
		}
		else
			g_pd3dDevice->SetTexture( 0, nullptr );
		if( this->pMesh->pMaterials )
			g_pd3dDevice->SetMaterial( &this->pMesh->pMaterials[i].MatD3D );

		// Draw
		this->pMesh->MeshData.pMesh->DrawSubset(i);
	}

	return S_OK;
}

Resource_Sound::Resource_Sound() : Resource()
{
	this->pBuffer = nullptr;
}
Resource_Sound::~Resource_Sound()
{
}

Resource_Light::Resource_Light() : Resource()
{
	this->Light.Type = D3DLIGHT_POINT;
	this->Light.Ambient.r = 0.0f;
	this->Light.Ambient.g = 0.0f;
	this->Light.Ambient.b = 0.0f;
	this->Light.Ambient.a = 1.0f;
	this->Light.Attenuation0 = 0.0f;
	this->Light.Attenuation1 = 0.01f;
	this->Light.Attenuation2 = 0.0f;
	this->Light.Diffuse.r = 0.8f;
	this->Light.Diffuse.g = 0.8f;
	this->Light.Diffuse.b = 0.8f;
	this->Light.Diffuse.a = 0.8f;
	this->Light.Direction.x = 0.0f;
	this->Light.Direction.y = 0.0f;
	this->Light.Direction.z = 1.0f;
	this->Light.Position.x = 1.0f;
	this->Light.Position.y = 20.0f;
	this->Light.Position.z = -20.0f;
	this->Light.Specular.r = 0.2f;
	this->Light.Specular.g = 0.2f;
	this->Light.Specular.b = 0.2f;
	this->Light.Specular.a = 1.0f;
	this->Light.Range = 100.f;
}



/********************************
	Request functions
********************************/

int __stdcall CreateMainMenu()
{
	if( g_pContext ) g_pContext->Destroy();
	g_pContext = new GOBJ_CONTEXT_MainMenu;
	g_pContext->Initialise();
	g_pContext->Create();

	return S_OK;
}
int __stdcall CreateHelpScreen()
{
	if( g_pContext ) g_pContext->Destroy();
	g_pContext = new GOBJ_CONTEXT_HelpScreen;
	g_pContext->Initialise();
	g_pContext->Create();

	return S_OK;
}
int __stdcall CreateOptionsMenu()
{
	if( g_pContext ) g_pContext->Destroy();
	g_pContext = new GOBJ_CONTEXT_OptionsMenu;
	g_pContext->Initialise();
	g_pContext->Create();

	return S_OK;
}
int __stdcall CreatePauseMenu()
{
	GOBJ_CONTEXT_PauseMenu * pPauseMenu = new GOBJ_CONTEXT_PauseMenu;
	pPauseMenu->Initialise();
	pPauseMenu->Create();
	pPauseMenu->pPrevious = g_pContext;
	g_pContext = pPauseMenu;

	return S_OK;
}
int __stdcall PauseMenuRestorePrevious()
{
	GOBJ_CONTEXT_PauseMenu *pPauseMenu = (GOBJ_CONTEXT_PauseMenu *)g_pContext;
	g_pContext = pPauseMenu->pPrevious;
	pPauseMenu->pPrevious = nullptr;
	pPauseMenu->Destroy();

	return S_OK;
}
void __stdcall DrawLoadScreen()
{
	float w = float(g_ClientRect.right-g_ClientRect.left)*0.5f,
		h = float(g_ClientRect.bottom-g_ClientRect.top)*0.5f;

	RECT rctLS =
	{
		long( w-200.f ),
		long( h-100.f ),
		long( w+200.f ),
		long( h+100.f )
	};
	IDirect3DSurface9 *pSurface;
	g_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface );
	g_pd3dDevice->ColorFill( pSurface, &rctLS, 0xff221100 );
	g_Sprite->Begin( D3DXSPRITE_ALPHABLEND );
	g_Font->DrawTextA( g_Sprite, "Loading...\n\nPlease wait...", -1, &rctLS, 0, 0xffffffff );
	g_Sprite->End();
	g_pd3dDevice->Present( 0, 0, 0, 0 );
	pSurface->Release();
}
void GOBJ_CONTEXT_MainGame::Congratulations()
{
	float w = float(g_ClientRect.right-g_ClientRect.left)*0.5f,
		h = float(g_ClientRect.bottom-g_ClientRect.top)*0.5f;

	RECT rctLS =
	{
		long( w-400.f ),
		long( h-100.f ),
		long( w+400.f ),
		long( h+100.f )
	};
	IDirect3DSurface9 *pSurface;
	g_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface );

	char text[512];
	sprintf_s(text,512,"Congratulations.\n\n"
		"Score: %i.\n\n"
		"Press ENTER to proceed to next level.",
		this->score);
	g_pd3dDevice->ColorFill( pSurface, &rctLS, 0xff88ccff );
	g_Sprite->Begin( D3DXSPRITE_ALPHABLEND );
	g_Font->DrawTextA( g_Sprite, text, -1, &rctLS, 0, 0xff773300 );
	g_Sprite->End();
	g_pd3dDevice->Present( 0, 0, 0, 0 );
	pSurface->Release();

	while( true )
	{
		MSG msg;
		PeekMessage( &msg, 0, 0, 0, 0 );
		TranslateMessage( &msg );
		if( GetAsyncKeyState(VK_RETURN) & 0x8000 ) break;
		Sleep(1);
	}

	g_Queue.AddRequest( this->OnLevelCompletion );
}
void GOBJ_CONTEXT_MainGame::TimeoutGameover()
{
	float w = float(g_ClientRect.right-g_ClientRect.left)*0.5f,
		h = float(g_ClientRect.bottom-g_ClientRect.top)*0.5f;

	RECT rctLS =
	{
		long( w-400.f ),
		long( h-100.f ),
		long( w+400.f ),
		long( h+100.f )
	};
	IDirect3DSurface9 *pSurface;
	g_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface );

	char text[512];
	sprintf_s(text,512,"Time\'s up! Game over.\n\n"
		"Score: %i.\n\n"
		"Press ENTER to exit.",
		this->score);
	g_pd3dDevice->ColorFill( pSurface, &rctLS, 0xffff8888 );
	g_Sprite->Begin( D3DXSPRITE_ALPHABLEND );
	g_Font->DrawTextA( g_Sprite, text, -1, &rctLS, 0, 0xff000000 );
	g_Sprite->End();
	g_pd3dDevice->Present( 0, 0, 0, 0 );
	pSurface->Release();

	while( true )
	{
		MSG msg;
		PeekMessage( &msg, 0, 0, 0, 0 );
		TranslateMessage( &msg );
		if( GetAsyncKeyState(VK_RETURN) & 0x8000 ) break;
		Sleep(1);
	}

	g_Queue.AddRequest( CreateMainMenu );
}
void GOBJ_CONTEXT_MainGame::NoLivesGameover()
{
	float w = float(g_ClientRect.right-g_ClientRect.left)*0.5f,
		h = float(g_ClientRect.bottom-g_ClientRect.top)*0.5f;

	RECT rctLS =
	{
		long( w-400.f ),
		long( h-100.f ),
		long( w+400.f ),
		long( h+100.f )
	};
	IDirect3DSurface9 *pSurface;
	g_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface );

	char text[512];
	sprintf_s(text,512,"No more lives! Game over.\n\n"
		"Score: %i.\n\n"
		"Press ENTER to exit.",
		this->score);
	g_pd3dDevice->ColorFill( pSurface, &rctLS, 0xffff8888 );
	g_Sprite->Begin( D3DXSPRITE_ALPHABLEND );
	g_Font->DrawTextA( g_Sprite, text, -1, &rctLS, 0, 0xff000000 );
	g_Sprite->End();
	g_pd3dDevice->Present( 0, 0, 0, 0 );
	pSurface->Release();

	while( true )
	{
		MSG msg;
		PeekMessage( &msg, 0, 0, 0, 0 );
		TranslateMessage( &msg );
		if( GetAsyncKeyState(VK_RETURN) & 0x8000 ) break;
		Sleep(1);
	}

	g_Queue.AddRequest( CreateMainMenu );
}
int __stdcall CreateLevel1()
{
	GOBJ_CONTEXT_MainGame *&Context = *(GOBJ_CONTEXT_MainGame**)&g_pContext;

	DrawLoadScreen();
	
	if( g_pContext )
	{
		if( g_pContext->GetObjId() != GOBJID_CONTEXT_MainGame )
		{
			g_pContext->Destroy();
			g_pContext = new GOBJ_CONTEXT_MainGame;
			Context->Initialise();
			Context->Create();
		}
		else
		{
			for( DWORD i = 0; i < Context->ListSize; i++ )
			{
				if( Context->ObjectList[i] )
				{
					Context->ObjectList[i]->Destroy();
					Context->ObjectList[i] = 0;
				}
			}
		}
	}
	else
	{
		g_pContext = new GOBJ_CONTEXT_MainGame;
		Context->Initialise();
		Context->Create();
	}
	Context->OnLevelCompletion = CreateLevel2;
	Context->TileWidth = 4;
	Context->TileHeight = 4;
	Context->dwTimer = 600;
	Context->dwLives = 2;

	Context->vecFarView = D3DXVECTOR3(0.0f,10.0f,-12.5f);
	g_Camera.SetPosition( &Context->vecFarView );

	GOBJ_GAME_MowerMover *pMower = new GOBJ_GAME_MowerMover;
	pMower->Initialise();
	pMower->Create();
	Context->RegisterObject( pMower );

	for( UINT i = 0; i < 8; i++ )
	{
		for( UINT j = 0; j < 8; j++ )
		{
			GOBJ_GAME_GrassTile *pObj = new GOBJ_GAME_GrassTile;
			pObj->pMesh = nullptr;
			pObj->Position[0] = float(i*2)-7.0f;
			pObj->Position[1] = 0.0f;
			pObj->Position[2] = float(j*2)-7.0f;
			pObj->IsMowed = false;
			pObj->Create();
			Context->RegisterObject( pObj );
		}
	}

	return S_OK;
}
int __stdcall CreateLevel2()
{
	GOBJ_CONTEXT_MainGame *&Context = *(GOBJ_CONTEXT_MainGame**)&g_pContext;

	DrawLoadScreen();
	
	if( g_pContext )
	{
		if( g_pContext->GetObjId() != GOBJID_CONTEXT_MainGame )
		{
			g_pContext->Destroy();
			g_pContext = new GOBJ_CONTEXT_MainGame;
			Context->Initialise();
			Context->Create();
		}
		else
		{
			for( DWORD i = 0; i < Context->ListSize; i++ )
			{
				if( Context->ObjectList[i] )
				{
					Context->ObjectList[i]->Destroy();
					Context->ObjectList[i] = 0;
				}
			}
		}
	}
	else
	{
		g_pContext = new GOBJ_CONTEXT_MainGame;
		Context->Initialise();
		Context->Create();
	}
	Context->OnLevelCompletion = CreateLevel3;
	Context->TileWidth = 6;
	Context->TileHeight = 6;
	Context->dwTimer = 900;
	Context->dwLives = 2;

	Context->vecFarView = D3DXVECTOR3(0.0f,15.0f,-18.75f);
	g_Camera.SetPosition( &Context->vecFarView );

	GOBJ_GAME_MowerMover *pMower = new GOBJ_GAME_MowerMover;
	pMower->Initialise();
	pMower->Create();
	Context->RegisterObject( pMower );

	for( UINT i = 0; i < 12; i++ )
	{
		for( UINT j = 0; j < 12; j++ )
		{
			GOBJ_GAME_GrassTile *pObj = new GOBJ_GAME_GrassTile;
			pObj->pMesh = nullptr;
			pObj->Position[0] = float(i*2)-11.0f;
			pObj->Position[1] = 0.0f;
			pObj->Position[2] = float(j*2)-11.0f;
			pObj->IsMowed = false;
			pObj->Create();
			Context->RegisterObject( pObj );
		}
	}

	return S_OK;
}
int __stdcall CreateLevel3()
{
	GOBJ_CONTEXT_MainGame *&Context = *(GOBJ_CONTEXT_MainGame**)&g_pContext;

	DrawLoadScreen();
	
	if( g_pContext )
	{
		if( g_pContext->GetObjId() != GOBJID_CONTEXT_MainGame )
		{
			g_pContext->Destroy();
			g_pContext = new GOBJ_CONTEXT_MainGame;
			Context->Initialise();
			Context->Create();
		}
		else
		{
			for( DWORD i = 0; i < Context->ListSize; i++ )
			{
				if( Context->ObjectList[i] )
				{
					Context->ObjectList[i]->Destroy();
					Context->ObjectList[i] = 0;
				}
			}
		}
	}
	else
	{
		g_pContext = new GOBJ_CONTEXT_MainGame;
		Context->Initialise();
		Context->Create();
	}
	Context->OnLevelCompletion = CreateLevel4;
	Context->TileWidth = 8;
	Context->TileHeight = 8;
	Context->dwTimer = 1200;
	Context->dwLives = 2;

	Context->vecFarView = D3DXVECTOR3(0.0f,20.0f,-25.0f);
	g_Camera.SetPosition( &Context->vecFarView );

	GOBJ_GAME_MowerMover *pMower = new GOBJ_GAME_MowerMover;
	pMower->Initialise();
	pMower->Create();
	Context->RegisterObject( pMower );

	for( UINT i = 0; i < 16; i++ )
	{
		for( UINT j = 0; j < 16; j++ )
		{
			GOBJ_GAME_GrassTile *pObj = new GOBJ_GAME_GrassTile;
			pObj->pMesh = nullptr;
			pObj->Position[0] = float(i*2)-15.0f;
			pObj->Position[1] = 0.0f;
			pObj->Position[2] = float(j*2)-15.0f;
			pObj->IsMowed = false;
			pObj->Create();
			Context->RegisterObject( pObj );
		}
	}

	return S_OK;
}
int __stdcall CreateLevel4()
{
	GOBJ_CONTEXT_MainGame *&Context = *(GOBJ_CONTEXT_MainGame**)&g_pContext;

	DrawLoadScreen();

	if( g_pContext )
	{
		if( g_pContext->GetObjId() != GOBJID_CONTEXT_MainGame )
		{
			g_pContext->Destroy();
			g_pContext = new GOBJ_CONTEXT_MainGame;
			Context->Initialise();
			Context->Create();
		}
		else
		{
			for( DWORD i = 0; i < Context->ListSize; i++ )
			{
				if( Context->ObjectList[i] )
				{
					Context->ObjectList[i]->Destroy();
					Context->ObjectList[i] = 0;
				}
			}
		}
	}
	else
	{
		g_pContext = new GOBJ_CONTEXT_MainGame;
		Context->Initialise();
		Context->Create();
	}
	Context->OnLevelCompletion = CreateEndGame;
	Context->TileWidth = 10;
	Context->TileHeight = 10;
	Context->dwTimer = 1500;
	Context->dwLives = 2;

	Context->vecFarView = D3DXVECTOR3(0.0f,25.0f,-31.25f);
	g_Camera.SetPosition( &Context->vecFarView );

	GOBJ_GAME_MowerMover *pMower = new GOBJ_GAME_MowerMover;
	pMower->Initialise();
	pMower->Create();
	Context->RegisterObject( pMower );

	for( UINT i = 0; i < 20; i++ )
	{
		for( UINT j = 0; j < 20; j++ )
		{
			GOBJ_GAME_GrassTile *pObj = new GOBJ_GAME_GrassTile;
			pObj->pMesh = nullptr;
			pObj->Position[0] = float(i*2)-19.0f;
			pObj->Position[1] = 0.0f;
			pObj->Position[2] = float(j*2)-19.0f;
			pObj->IsMowed = false;
			pObj->Create();
			Context->RegisterObject( pObj );
		}
	}

	return S_OK;
}
int __stdcall CreateEndGame()
{
	GOBJ_CONTEXT_MainGame *&Context = *(GOBJ_CONTEXT_MainGame**)&g_pContext;
	
	if( g_pContext )
	{
		IDirect3DSurface9 *pSurface;
		g_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface );

		char text[512];
		sprintf_s(text,512,"You have completed the game.\n\n"
			"Final score: %i.\n\n"
			"Thanks for playing!\n\n\n"
			"Press SPACE to return to the Start Menu.",
			Context->score);
		g_pd3dDevice->ColorFill( pSurface, &g_ClientRect, 0xffff8877 );
		g_Font->DrawTextA( NULL, text, -1, &g_ClientRect, 0, 0xff007788 );
		g_pd3dDevice->Present( 0, 0, 0, 0 );
		pSurface->Release();

		while( true )
		{
			MSG msg;
			PeekMessage( &msg, 0, 0, 0, 0 );
			if( GetAsyncKeyState(VK_SPACE) & 0x8000 ) break;
			Sleep(1);
		}
	}

	g_Queue.AddRequest( CreateMainMenu );

	return S_OK;
}



/********************************
	Other functions
********************************/

HRESULT LoadEmbeddedMesh(Resource_Mesh * pOut, LPSTR ResourceName)
{
	HMODULE hModule = GetModuleHandleA(0);
	HRSRC hResInfo = FindResourceA( hModule, ResourceName, "RSRC" );
	HGLOBAL hRes = LoadResource( hModule, hResInfo );

	// Allocate
	pOut->pMesh = new(std::nothrow) D3DXMESHCONTAINER();
	if( !pOut->pMesh ) return E_OUTOFMEMORY;

	// Load mesh
	LPD3DXBUFFER pBuffer;
	if( FAILED( D3DXLoadMeshFromXInMemory( hRes,
		SizeofResource( hModule, hResInfo ),
		D3DXMESH_MANAGED,
		g_pd3dDevice,
		NULL,
		&pBuffer,
		NULL,
		&pOut->pMesh->NumMaterials,
		&pOut->pMesh->MeshData.pMesh ) ) )
	{
		MessageBoxA( g_hWnd, "Failed to load mesh.", WindowTitle, MB_ICONHAND );
		return E_FAIL;
	}

	FreeResource( hRes );

	LPD3DXMATERIAL pMats = (LPD3DXMATERIAL)pBuffer->GetBufferPointer();

	// Allocate buffers
	pOut->pMesh->pMaterials = new(std::nothrow) D3DXMATERIAL[pOut->pMesh->NumMaterials];
	pOut->ppTextures = new Resource_Texture *[pOut->pMesh->NumMaterials]();

	// Copy materials
	for( DWORD i = 0; i < pOut->pMesh->NumMaterials; i++ )
	{
		// Set ambient component equal to diffuse
		pOut->pMesh->pMaterials[i] = pMats[i];
		pOut->pMesh->pMaterials[i].MatD3D.Ambient =
			pOut->pMesh->pMaterials[i].MatD3D.Diffuse;

		// Attempt to load texture only if there is a texture filename
		if( pOut->pMesh->pMaterials[i].pTextureFilename )
		{
			// Is texture already a resource?
			Resource_Texture * rTexture;
			if( rTexture = (Resource_Texture *)g_Resource.GetResourceByName(
				pOut->pMesh->pMaterials[i].pTextureFilename ) )
			{
				pOut->ppTextures[i] = rTexture;
				rTexture->AddRef();
			}
			else
			{
				// Get resource id
				DWORD ID = GetResourceIntByName(
					pOut->pMesh->pMaterials[i].pTextureFilename );

				// Open resource
				hResInfo = FindResource( hModule, MAKEINTRESOURCE(ID), "RSRC" );

				if( hResInfo )
				{
					hRes = LoadResource( hModule, hResInfo );

					// Load texture
					IDirect3DTexture9 * pTexture;
					if( SUCCEEDED( D3DXCreateTextureFromFileInMemory(
						g_pd3dDevice,
						hRes,
						SizeofResource(hModule,hResInfo),
						&pTexture ) ) )
					{
						// Release embedded resource
						FreeResource( hRes );

						// Create texture resource
						rTexture = new(std::nothrow) Resource_Texture();
						if( !rTexture )
						{
							pTexture->Release();
							pOut->ppTextures[i]->Release();
							pBuffer->Release();
							return E_OUTOFMEMORY;
						}
						rTexture->pTexture = pTexture;
						g_Resource.AddResource(
							rTexture,
							pOut->pMesh->pMaterials[i].pTextureFilename );
						pOut->ppTextures[i] = rTexture;
					}
					else
						pOut->ppTextures[i] = nullptr;
				}
				else
					pOut->ppTextures[i] = nullptr;
			}
		}
		else
			pOut->ppTextures[i] = nullptr;
	}

	// Release buffer
	pBuffer->Release();

	return S_OK;
}

HRESULT LoadEmbeddedWAV( Resource_Sound * pOut, LPSTR ResourceName )
{
	HMODULE hModule = GetModuleHandleA(0);
	HRSRC hResInfo = FindResourceA( hModule, ResourceName, "RSRC" );
	HGLOBAL hRes = LoadResource( hModule, hResInfo );
	LPVOID pReadPosition = hRes;
	DWORD& dwReadPosition = *((DWORD*)&pReadPosition);

	if( !hRes ) return E_FAIL;

	// Read header
	WaveFileHeader& header = *((WaveFileHeader*)pReadPosition);
	dwReadPosition += sizeof(WaveFileHeader);

	// Verify
	if( header.id[0] != 'R' ||
		header.id[1] != 'I' ||
		header.id[2] != 'F' ||
		header.id[3] != 'F' )
	{
		MessageBoxA( g_hWnd, "Failed to load .WAV file.\nInvalid header.", WindowTitle, MB_ICONHAND );
		FreeResource( hRes );
		return E_INVALIDARG;
	}
	if( header.header[0] != 'W' ||
		header.header[1] != 'A' ||
		header.header[2] != 'V' ||
		header.header[3] != 'E' )
	{
		MessageBoxA( g_hWnd, "Failed to load .WAV file.\nUnsupported .WAV file type.", WindowTitle, MB_ICONHAND );
		FreeResource( hRes );
		return E_INVALIDARG;
	}
	if( header.assert[0] != 'f' ||
		header.assert[1] != 'm' ||
		header.assert[2] != 't' ||
		header.assert[3] != 32 )
	{
		MessageBoxA( g_hWnd, "Failed to load .WAV file.\nUnsupported .WAV file type.", WindowTitle, MB_ICONHAND );
		FreeResource( hRes );
		return E_INVALIDARG;
	}
	if( header.dwLenFmtData != 16 )
	{
		MessageBoxA( g_hWnd, "Failed to load .WAV file.\nUnexpected data occurence.", WindowTitle, MB_ICONHAND );
		FreeResource( hRes );
		return E_INVALIDARG;
	}

	// Import data
	WAVEFORMATEX WaveFormat;
	WaveFormat.cbSize = 0;
	WaveFormat.nAvgBytesPerSec = header.dwAvgBytesPerSec;
	WaveFormat.nBlockAlign = header.wBlockAlign;
	WaveFormat.nChannels = header.dwNumChannels;
	WaveFormat.nSamplesPerSec = header.dwSampleRate;
	WaveFormat.wBitsPerSample = header.dwBitsPerSample;
	WaveFormat.wFormatTag = header.wFmtType;

	// Parse chunk
	WaveDataChunk chnk;
	while( true )
	{
		chnk = *((WaveDataChunk*)pReadPosition);
		dwReadPosition += sizeof(WaveDataChunk);
		if( chnk.ChunkName[0] == 'd' &&
			chnk.ChunkName[1] == 'a' &&
			chnk.ChunkName[2] == 't' &&
			chnk.ChunkName[3] == 'a' )
		{
			break;
		}
		else
		{
			dwReadPosition += chnk.dwSizeOfChunk;
		}
	}

	// Initialise sound buffer
	DSBUFFERDESC WaveBufferDesc;
	WaveBufferDesc.dwReserved = 0;
	WaveBufferDesc.dwSize = sizeof(DSBUFFERDESC);
	WaveBufferDesc.dwBufferBytes = chnk.dwSizeOfChunk;
	WaveBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY;
	WaveBufferDesc.lpwfxFormat = &WaveFormat;
	WaveBufferDesc.guid3DAlgorithm = GUID_NULL;

	if( pOut->pBuffer ) pOut->pBuffer->Release();
	if( FAILED( g_pSound->CreateSoundBuffer(
		&WaveBufferDesc,
		&pOut->pBuffer,
		NULL ) ) )
	{
		MessageBoxA( g_hWnd, "Direct Sound error:\nFailed to create sound buffer.", WindowTitle, MB_ICONHAND );
		FreeResource( hRes );
		return E_FAIL;
	}

	void * ptrLock;
	DWORD dwLockSize;
	if( FAILED( pOut->pBuffer->Lock(
		0, 0, &ptrLock, &dwLockSize, 0, 0,
		DSBLOCK_ENTIREBUFFER ) ) )
	{
		FreeResource( hRes );
		return E_FAIL;
	}
	memcpy( ptrLock, pReadPosition, dwLockSize );
	pOut->pBuffer->Unlock( ptrLock, dwLockSize, 0, 0 );

	FreeResource( hRes );
	
	return S_OK;
}

DWORD GetResourceIntByName( LPSTR Name )
{
	if( strcmp( Name, "Button_Active.png" ) == 0 ) {
		return IDR_STR_ButtonActive;
	} else if( strcmp( Name, "Button_Disabled.png" ) == 0 ) {
		return IDR_STR_ButtonDisabled;
	} else if( strcmp( Name, "Button_Pressed.png" ) == 0 ) {
		return IDR_STR_ButtonPressed;
	} else if( strcmp( Name, "Grass Blade.jpg" ) == 0 ) {
		return IDR_STR_GrassBlade;
	} else if( strcmp( Name, "Seamless_grass.jpg" ) == 0 ) {
		return IDR_STR_GrassSeamless;
	} else if( strcmp( Name, "Dirt.jpg" ) == 0 ) {
		return IDR_STR_Dirt;
	} else if( strcmp( Name, "SunPainting.jpg" ) == 0 ) {
		return IDR_STR_SunPainting;
	} else return 0;
}

