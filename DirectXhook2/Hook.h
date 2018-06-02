#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#ifndef __DX_API__
#define __DX_API__
#define DX_API HRESULT WINAPI
#endif // !__DX_API__

#include "Polyhook.h"

DX_API initialEndScenehk(LPDIRECT3DDEVICE9 device);

DX_API endScenehk(LPDIRECT3DDEVICE9 pDevice);

DX_API resethk(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS * pPresentationParameters);

//-----------------------those are testing DX hooks
void enableLighthackDirectional(LPDIRECT3DDEVICE9 pDevice);
//-----------------------those are testing DX hooks---------------//