#pragma once

#include "Memory.h"
#include<vector>

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")



#define DX_API HRESULT WINAPI

typedef HRESULT(WINAPI* _reset)(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
typedef HRESULT(WINAPI* _endScene)(LPDIRECT3DDEVICE9 pDevice);

class D3D9Hook;
typedef void(*_drawFrameCallback)(D3D9Hook* hook, LPDIRECT3DDEVICE9 pDevice);

class D3D9Hook
{
public:
	static _reset origReset;
	static _endScene origEndScene;

	static D3D9Hook* getInstance()
	{
		if (!D3D9Hook::instance)
			D3D9Hook::instance = new D3D9Hook;
		return D3D9Hook::instance;
	}

	static void deldeteInstance()
	{
		if (D3D9Hook::instance)
		{
			delete D3D9Hook::instance;
			D3D9Hook::instance = nullptr;
		}
	}
	void initialize();

	DWORD __stdcall reportInitEndScene(LPDIRECT3DDEVICE9 device);
	DWORD initHookCallback(LPDIRECT3DDEVICE9 device);

	void addDrawFrameCallback(_drawFrameCallback cb);

	DX_API resetHookCallback(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	DX_API endSceneHookCallback(LPDIRECT3DDEVICE9 pDevice);


private:
	D3D9Hook() {}
	~D3D9Hook() {}

	static LPDIRECT3DDEVICE9 gameDevice;         //to store game's d3d9 device

	static D3D9Hook* instance;
	static bool hookReadyPre, hookReady;

	static DWORD endSceneAddress;

	static unsigned char* originalAsm;     //putting it in public allows Hook::unhookWithJump to deldete[] it

	DWORD locateOrigEndSceneAddres();

	void placeHooks();
	void onLostDevice();

	std::vector<_drawFrameCallback> drawFrameCallbacks;

	struct VFHookInfo
	{
		VFHookInfo(DWORD _index, DWORD cb, DWORD* _origFunc) : index(_index), callback(cb), origFunc(_origFunc) {}
		DWORD index, callback;
		DWORD* origFunc;
	};

};
