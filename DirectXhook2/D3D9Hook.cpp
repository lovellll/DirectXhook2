#include "D3D9Hook.h"
#include "Hook.h"
#include "DebugConsole.h"
#include "Memory.h"
#include "Menu.h"
#include "Options.h"

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

LPD3DXFONT		  D3D9Hook::m_font = nullptr;

void D3D9Hook::initialize()
{
	while (!GetModuleHandleA("d3d9.dll"))
		Sleep(10);

	D3D9Hook::my_endSceneAddress = this->locateOrigEndSceneAddres();
#ifdef _DEBUG
	DebugConsole::ConsolePrint("endSceneAddress is %x\n", D3D9Hook::my_endSceneAddress);
#endif // _DEBUG

	if (D3D9Hook::my_endSceneAddress)
		//D3D9Hook::originalAsm = Hook::hookWithJump(D3D9Hook::my_endSceneAddress, (DWORD)&initialEndScenehk);  //store original endScene() asm code
	{
		Detour_initialEndScene = new PLH::Detour;
		Detour_initialEndScene->SetupHook((BYTE*)D3D9Hook::my_endSceneAddress, (BYTE*)&initialEndScenehk);

#ifdef _DEBUG
		DebugConsole::ConsolePrint("&initialEndScenehk is %x\n", &initialEndScenehk);
#endif // _DEBUG

		Detour_initialEndScene->Hook();
		initialOrigEndScene = (_endScene)D3D9Hook::my_endSceneAddress;
#ifdef _DEBUG
		DebugConsole::ConsolePrint("initialOrigEndScene is %x\n", initialOrigEndScene);
#endif // _DEBUG

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

#ifdef _DEBUG
	DebugConsole::ConsolePrint("device address is %x\n", pDevice);
#endif

	Detour_initialEndScene->UnHook();
	delete Detour_initialEndScene;
	//-----------------Initialize textures, fonts, etc...-----------------------------//
	//-----------------Initialize textures, fonts, etc...-----------------------------//
	this->placeHooks(pDevice);
	//D3D9Hook::hookReadyPre = true;
	return D3D9Hook::initialOrigEndScene(pDevice);   
}

void D3D9Hook::placeHooks(LPDIRECT3DDEVICE9 pDevice)
{
	//can place your hooks here
	//----------------------endScenehook-------------------------//
	VTableSwap_placeHooks = new PLH::VTableSwap;
	VTableSwap_placeHooks->SetupHook((BYTE*)gameDevice, 42, (BYTE*)&endScenehk);
	VTableSwap_placeHooks->Hook();
	origEndScene = VTableSwap_placeHooks->GetOriginal<_endScene>();
	//----------------------endScenehook-------------------------//
	//----------------------other hooks--------------------------//
	//origReset = VTableSwap_placeHooks->HookAdditional<_reset>(16, (BYTE*)&resethk);
	//origDrawIndexedPrimitive = VTableSwap_placeHooks->HookAdditional<_drawIndexedPrimitive>(82, (BYTE*)&drawIndexedPrimitivehk);
	//----------------------other hooks--------------------------//
	//----------------------hook WndProc-------------------------//
	Menu::getInstance()->initialize(pDevice);
	//----------------------hook WndProc-------------------------//
}

DWORD D3D9Hook::endSceneCallback(LPDIRECT3DDEVICE9 pDevice)
{
#ifdef _DEBUG
	//DebugConsole::ConsolePrint("program called our endSceneCallback!");
#endif // _DEBUG
	//drawText
	if (g_Options.text_enabled)
		drawText(60, 60, D3DCOLOR_ARGB(255, 0, 255, 0), "TEXT");

	//imgui menu
	Menu::getInstance()->Menu::imguiInitialize(pDevice);
	Menu::getInstance()->Menu::renderFrame();
	
	return origEndScene(pDevice);
}

DWORD D3D9Hook::resetCallback(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
#ifdef _DEBUG
	DebugConsole::ConsolePrint("program called our resetCallback!");
#endif // _DEBUG

	auto result = origReset(pDevice, pPresentationParameters);
	if (result == D3D_OK)
	{
		//put your own functions here
		this->onLostDevice();
	}
	return result;
}

HRESULT WINAPI D3D9Hook::drawIndexedPrimitiveCallback(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
#ifdef _DEBUG
	//DebugConsole::ConsolePrint("program called our drawIndexedPrimitiveCallback!");
#endif // _DEBUG
	//----------------------------close Z-buffing---------------------------//
	/*
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
	*/
	//----------------------------close Z-buffing---------------------------//
	return origDrawIndexedPrimitive(pDevice, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

void D3D9Hook::drawText(int x, int y, D3DCOLOR color, const char *text, ...)
{
	if (!m_font) 
		D3DXCreateFontA(gameDevice, 16, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &m_font);
	RECT rect;
	va_list va_alist;
	char buf[256];

	va_start(va_alist, text);
	_vsnprintf_s(buf, sizeof(buf), text, va_alist);
	va_end(va_alist);

	rect.left = x + 1;
	rect.top = y + 1;
	rect.right = rect.left + 1000;
	rect.bottom = rect.top + 1000;
	
	if (m_font)
	{
		this->m_font->DrawTextA(NULL, buf, -1, &rect, 0, D3DCOLOR_ARGB(255, 10, 10, 10));
		rect.left--;
		rect.top--;
		this->m_font->DrawTextA(NULL, buf, -1, &rect, 0, color);
	}
}

void D3D9Hook::onLostDevice(){}

