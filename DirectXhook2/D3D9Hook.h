#pragma once

#include<vector>
#include "Polyhook.h"

//----------------d3d9 SDK--------------
#include <d3d9.h>
#include <d3dx9.h>
//#pragma comment(lib, "d3d9.lib")
#ifdef _DEBUG
	#pragma comment(lib,"d3dx9d.lib")
#else
	#pragma comment(lib,"d3dx9.lib")
#endif
//----------------d3d9 SDK--------------

#ifndef __DX_API__
	#define __DX_API__
	#define DX_API HRESULT WINAPI
#endif // !__DX_API__

typedef HRESULT(WINAPI* _endScene)(LPDIRECT3DDEVICE9 pDevice);
typedef HRESULT(WINAPI* _reset)(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
typedef HRESULT(WINAPI* _drawIndexedPrimitive)(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount);



class D3D9Hook;

class D3D9Hook
{
public:
	//----------------device function address----------//
	static _endScene initialOrigEndScene;
	static _endScene origEndScene;
	static _reset origReset;
	static _drawIndexedPrimitive origDrawIndexedPrimitive;
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
	void drawIndexedPrimitiveCallback(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount);

	LPDIRECT3DTEXTURE9 addTexture(std::wstring imagePath);
	bool drawMessage(LPD3DXFONT font, unsigned int x, unsigned int y, int alpha, unsigned char r, unsigned char g, unsigned char b, LPCWSTR Message);

private:
	D3D9Hook() {}
	~D3D9Hook() {}

	//-----------------Detour dynamic allocated pointers--------------------
	static PLH::Detour* Detour_initialEndScene;
	static PLH::VTableSwap* VTableSwap_placeHooks;
	//-----------------Detour dynamic allocated pointers--------------------

	static LPDIRECT3DDEVICE9 gameDevice;         //to store game's d3d9 device
	static D3D9Hook* instance;
	static DWORD my_endSceneAddress;

	static bool hookReadyPre, hookReady;

	DWORD locateOrigEndSceneAddres();

	void placeHooks();
	void onLostDevice();

	static LPDIRECT3DTEXTURE9 addedTexture;
	static LPD3DXFONT		  m_font;
};
