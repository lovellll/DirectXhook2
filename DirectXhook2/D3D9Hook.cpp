#include "D3D9Hook.h"
#include "Hook.h"

#include "Polyhook.hpp"
#include "capstone.h"
#pragma comment(lib, "capstone.lib")

LPDIRECT3DDEVICE9 D3D9Hook::gameDevice = nullptr;
D3D9Hook* D3D9Hook::instance = nullptr;
bool D3D9Hook::hookReadyPre = false;
bool D3D9Hook::hookReady = false;

DWORD D3D9Hook::endSceneAddress = NULL;
unsigned char* D3D9Hook::originalAsm = nullptr;

_endScene D3D9Hook::origEndScene = nullptr;
_reset D3D9Hook::origReset = nullptr;

void D3D9Hook::initialize()
{
	while (!GetModuleHandleA("d3d9.dll"))
		Sleep(10);

	D3D9Hook::endSceneAddress = this->locateOrigEndSceneAddres();
#ifdef DEBUG
	DebugConsole::ConsolePrint("endSceneAddress is %x\n", D3D9Hook::endSceneAddress);
#endif // DEBUG

	if (D3D9Hook::endSceneAddress);
		//D3D9Hook::originalAsm = Hook::hookWithJump(D3D9Hook::endSceneAddress, (DWORD)&endScenehk);  //store original endScene() asm code
	{
		std::shared_ptr<PLH::Detour> Detour_Ex(new PLH::Detour);
		Detour_Ex->SetupHook((BYTE*)D3D9Hook::endSceneAddress, (BYTE*)&endScenehk);
		Detour_Ex->Hook();
		origEndScene = Detour_Ex->GetOriginal<_endScene>();
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

	endSceneAddress = Memory::getVF((DWORD)pd3dDevice, 42);
	pD3D->Release();
	pd3dDevice->Release();
	DestroyWindow(hWnd);//release our d3d9 Device
	return endSceneAddress;

}

/*
DWORD __stdcall D3D9Hook::reportInitEndScene(LPDIRECT3DDEVICE9 device)
{
	return D3D9Hook::getInstance()->initHookCallback(device);
}
*/

DWORD D3D9Hook::initHookCallback(LPDIRECT3DDEVICE9 pDevice)
{
	D3D9Hook::gameDevice = pDevice;

#ifdef DEBUG
	DebugConsole::ConsolePrint("device address is %x\n", pDevice);
#endif

	//while (D3D9Hook::originalAsm == NULL) {}
	//Hook::unhookWithJump(D3D9Hook::endSceneAddress, originalAsm);

	//D3DXCreateFont
	//this->placeHooks();
	//D3D9Hook::hookReadyPre = true;

	return D3D9Hook::origEndScene(pDevice);                   //return endSceneAddress so reportInitEndScene can put it in EAX
}

/*
void D3D9Hook::placeHooks()
{
	//static const DWORD VHHookCount = 2;
	static const DWORD VHHookCount = 1;
	static VFHookInfo VFHooks[VHHookCount] =
	{
		//VFHookInfo(16, (DWORD)&myReset, (DWORD*)&D3D9Hook::origReset),
		VFHookInfo(42, (DWORD)&myEndScene, (DWORD*)&D3D9Hook::origEndScene)
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


	auto result = origEndScene(pDevice);
	this->placeHooks();      //Ë³Ðò£¿
	return result;
}

void D3D9Hook::addDrawFrameCallback(_drawFrameCallback cb)
{
	if (!D3D9Hook::hookReady)
		this->drawFrameCallbacks.push_back(cb);
}
*/