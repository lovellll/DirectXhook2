#include "D3D9Hook.h"+
#include "Hook.h"

DX_API initialEndScenehk(LPDIRECT3DDEVICE9 pDevice)
{
	return D3D9Hook::getInstance()->initHookCallback(pDevice);
}

DX_API endScenehk(LPDIRECT3DDEVICE9 pDevice)
{
	return D3D9Hook::getInstance()->endSceneCallback(pDevice);
}