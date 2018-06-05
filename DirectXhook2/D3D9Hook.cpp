#include "D3D9Hook.h"
#include "Hook.h"
#include "DebugConsole.h"
#include "Memory.h"
#include <stdio.h>
#include <stdarg.h>

//-----------------Detour dynamic allocated pointers--------------------
PLH::Detour* D3D9Hook::Detour_initialEndScene = nullptr;
PLH::VTableSwap* D3D9Hook::VTableSwap_placeHooks = nullptr;
//-----------------Detour dynamic allocated pointers--------------------

LPDIRECT3DDEVICE9 D3D9Hook::gameDevice = nullptr;
D3D9Hook* D3D9Hook::instance = nullptr;
bool D3D9Hook::hookReadyPre = false;
bool D3D9Hook::hookReady = false;

DWORD D3D9Hook::my_endSceneAddress = NULL;

_endScene D3D9Hook::initialOrigEndScene = nullptr;
_endScene D3D9Hook::origEndScene = nullptr;
_reset D3D9Hook::origReset = nullptr;
_drawIndexedPrimitive D3D9Hook::origDrawIndexedPrimitive = nullptr;


void D3D9Hook::initialize()
{
	while (!GetModuleHandleA("d3d9.dll"))
		Sleep(10);

	D3D9Hook::my_endSceneAddress = this->locateOrigEndSceneAddres();
#ifdef DEBUG
	DebugConsole::ConsolePrint("endSceneAddress is %x\n", D3D9Hook::my_endSceneAddress);
#endif // DEBUG

	if (D3D9Hook::my_endSceneAddress)
		//D3D9Hook::originalAsm = Hook::hookWithJump(D3D9Hook::my_endSceneAddress, (DWORD)&initialEndScenehk);  //store original endScene() asm code
	{
		Detour_initialEndScene = new PLH::Detour;
		Detour_initialEndScene->SetupHook((BYTE*)D3D9Hook::my_endSceneAddress, (BYTE*)&initialEndScenehk);

#ifdef DEBUG
		DebugConsole::ConsolePrint("&initialEndScenehk is %x\n", &initialEndScenehk);
#endif // DEBUG

		Detour_initialEndScene->Hook();
		//initialOrigEndScene = Detour_initialEndScene->GetOriginal<_endScene>();
		initialOrigEndScene = (_endScene)D3D9Hook::my_endSceneAddress;
#ifdef DEBUG
		DebugConsole::ConsolePrint("initialOrigEndScene is %x\n", initialOrigEndScene);
#endif // DEBUG

	}																								
	D3D9Hook::hookReady = true;
}

//to get endScene vftable address
DWORD D3D9Hook::locateOrigEndSceneAddres()
{
	WNDCLASSEXA wc =
	{
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		DefWindowProc,
		0L,0L,
		GetModuleHandleA(NULL),
		NULL, NULL, NULL, NULL,
		"DX", NULL
	};

	RegisterClassExA(&wc);
	HWND hWnd = CreateWindowA("DX", NULL, WS_OVERLAPPEDWINDOW, 100, 100, 600, 600, GetDesktopWindow(), NULL, wc.hInstance, NULL);

	LPDIRECT3D9 pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (!pD3D)
		return 0;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;

	LPDIRECT3DDEVICE9 pd3dDevice;
	HRESULT res = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pd3dDevice);

	if (FAILED(res))
		return 0;

	my_endSceneAddress = Memory::getVF((DWORD)pd3dDevice, 42);
	pD3D->Release();
	pd3dDevice->Release();
	DestroyWindow(hWnd);//release our d3d9 Device
	return my_endSceneAddress;

}

DWORD D3D9Hook::initHookCallback(LPDIRECT3DDEVICE9 pDevice)
{
	D3D9Hook::gameDevice = pDevice;

#ifdef DEBUG
	DebugConsole::ConsolePrint("device address is %x\n", pDevice);
#endif

	Detour_initialEndScene->UnHook();
	delete Detour_initialEndScene;
	//-----------------Initialize textures, fonts, etc...-----------------------------//
	addedTexture = this->addTexture(L"red.png");
	D3DXCreateFont(gameDevice, 15, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial", &this->addedfont);
	D3D9Hook::hookReadyPre = true;
	//-----------------Initialize textures, fonts, etc...-----------------------------//
	this->placeHooks();
	D3D9Hook::hookReady = true;
	return D3D9Hook::initialOrigEndScene(pDevice);   
}

void D3D9Hook::placeHooks()
{
	//can place your hooks here
	//D3D9Hook::gameDevice is the class
	//----------------------endScenehook-------------------------//
	VTableSwap_placeHooks = new PLH::VTableSwap;
	VTableSwap_placeHooks->SetupHook((BYTE*)gameDevice, 42, (BYTE*)&endScenehk);
	VTableSwap_placeHooks->Hook();
	origEndScene = VTableSwap_placeHooks->GetOriginal<_endScene>();
	//----------------------endScenehook-------------------------//
	origReset = VTableSwap_placeHooks->HookAdditional<_reset>(16, (BYTE*)&resethk);
	origDrawIndexedPrimitive = VTableSwap_placeHooks->HookAdditional<_drawIndexedPrimitive>(82, (BYTE*)&drawIndexedPrimitivehk);
}

DWORD D3D9Hook::endSceneCallback(LPDIRECT3DDEVICE9 pDevice)
{
#ifdef DEBUG
	//DebugConsole::ConsolePrint("program called our endSceneCallback!");
#endif // DEBUG
	//put your own functions here
	if (D3D9Hook::hookReady = true)
	{
		enableLighthackDirectional(pDevice);
		enableLightHackAmbient(pDevice);
	}
	if (D3D9Hook::hookReadyPre = true)
	{
		drawText(10, 25, D3DCOLOR_ARGB(255, 255, 0, 0), "Draw text test");
	}

	this->placeHooks();
	D3D9Hook::hookReady = true;

	return origEndScene(pDevice);
}

DWORD D3D9Hook::resetCallback(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
#ifdef DEBUG
	DebugConsole::ConsolePrint("program called our resetCallback!");
#endif // DEBUG

	D3D9Hook::hookReadyPre = false;
	auto result = origReset(pDevice, pPresentationParameters);
	if (result == D3D_OK)
	{
		//put your own functions here
		this->onLostDevice();
	}
	return result;
}

void D3D9Hook::drawIndexedPrimitiveCallback(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
#ifdef DEBUG
	//DebugConsole::ConsolePrint("program called our drawIndexedPrimitiveCallback!");
#endif // DEBUG
	//----------------------------close Z-buffing---------------------------//
	if (NumVertices == 24 && primCount == 12)
	{
		pDevice->SetRenderState(D3DRS_ZENABLE, false);
		origDrawIndexedPrimitive(pDevice, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
		pDevice->SetRenderState(D3DRS_ZENABLE, true);
	}
	else
	{
		origDrawIndexedPrimitive(pDevice, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
	}
	//----------------------------close Z-buffing---------------------------//
	//----------------------------change texture---------------------------//
	pDevice->SetTexture(0, addedTexture);
	//----------------------------change texture---------------------------//
}

LPDIRECT3DTEXTURE9 D3D9Hook::addTexture(std::wstring imagePath)
{
	LPDIRECT3DTEXTURE9 texture;
	if(D3DXCreateTextureFromFile(this->gameDevice, imagePath.c_str(), &texture) < 0)
		return NULL;
	return texture;
}

void D3D9Hook::drawText(int x, int y, D3DCOLOR color, const char *text, ...)
{
	//THIS FUNCTION WILL CRASH!//
	RECT rect;
	va_list va_alist;
	char buf[256];

	va_start(va_alist, text);
	_vsnprintf_s(buf, sizeof(buf), text, va_alist);
	va_end(va_alist);

	rect.left = x + 1;
	rect.top = y + 1;
	rect.right = rect.left + 100;
	rect.bottom = rect.top + 100;


	this->addedfont->DrawTextA(NULL, buf, -1, &rect, 0, D3DCOLOR_ARGB(255, 10, 10, 10));
	rect.left--;
	rect.top--;
	this->addedfont->DrawTextA(NULL, buf, -1, &rect, 0, color);
}

void D3D9Hook::onLostDevice()
{
	//addedTexture = this->addTexture(L"red.png");
	if (this->addedfont)
		this->addedfont->OnLostDevice();
	D3D9Hook::hookReadyPre = true;
}

/*
void D3D9Hook::placeHooks()
{
	//static const DWORD VHHookCount = 2;
	static const DWORD VHHookCount = 1;
	static VFHookInfo VFHooks[VHHookCount] =
	{
		//VFHookInfo(16, (DWORD)&myReset, (DWORD*)&D3D9Hook::origReset),
		VFHookInfo(42, (DWORD)&myEndScene, (DWORD*)&D3D9Hook::initialOrigEndScene)
	};

	for (int hook = 0; hook < VHHookCount; hook++)
	{
		DWORD ret = Hook::vfHook((DWORD)D3D9Hook::gameDevice, VFHooks[hook].index, VFHooks[hook].callback);
		if (ret != VFHooks[hook].callback)
			*VFHooks[hook].origFunc = ret;
	}
}

DX_API D3D9Hook::resetHookCallback(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS * pPresentationParameters)
{
	auto result = origReset(pDevice, pPresentationParameters);
	if (result == D3D_OK)
		this->onLostDevice();
	return result;
}
void D3D9Hook::onLostDevice()
{
	//call your recovery functios here
#ifdef DEBUG
	DebugConsole::ConsolePrint("device reset!\n");
#endif // DEBUG

}

DX_API D3D9Hook::endSceneHookCallback(LPDIRECT3DDEVICE9 pDevice)
{
	
	//for (int i = 0; i < drawFrameCallbacks.size(); i++)
	//drawFrameCallbacks[i](this, pDevice);
	
#ifdef DEBUG
	//DebugConsole::ConsolePrint("gamedevice's endScene Attached!\n");
#endif // DEBUG


	auto result = initialOrigEndScene(pDevice);
	this->placeHooks();      //Ë³Ðò£¿
	return result;
}

void D3D9Hook::addDrawFrameCallback(_drawFrameCallback cb)
{
	if (!D3D9Hook::hookReady)
		this->drawFrameCallbacks.push_back(cb);
}
*/