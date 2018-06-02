#pragma once

#include<vector>
#include "Polyhook.h"

//----------------d3d9 SDK--------------
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
//----------------d3d9 SDK--------------

#ifndef __DX_API__
	#define __DX_API__
	#define DX_API HRESULT WINAPI
#endif // !__DX_API__


typedef HRESULT(WINAPI* _reset)(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
typedef HRESULT(WINAPI* _endScene)(LPDIRECT3DDEVICE9 pDevice);

class D3D9Hook;

class D3D9Hook
{
public:
	//----------------device function address----------//
	static _endScene initialOrigEndScene;
	static _endScene origEndScene;
	static _reset origReset;
	//----------------device function address----------//

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

	DWORD initHookCallback(LPDIRECT3DDEVICE9 pDevice);
	DWORD endSceneCallback(LPDIRECT3DDEVICE9 pDevice);
	DWORD resetCallback(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);

private:
	D3D9Hook() {}
	~D3D9Hook() {}

	//-----------------Detour dynamic allocated pointers--------------------
	static PLH::Detour* Detour_initialEndScene;
	static PLH::VTableSwap* VTableSwap_placeHooks;
	//-----------------Detour dynamic allocated pointers--------------------

	static LPDIRECT3DDEVICE9 gameDevice;         //to store game's d3d9 device

	static D3D9Hook* instance;
	static bool hookReadyPre, hookReady;

	static DWORD my_endSceneAddress;

	DWORD locateOrigEndSceneAddres();

	void placeHooks();

	void onLostDevice();

};
