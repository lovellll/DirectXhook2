#include "D3D9Hook.h"+

DX_API endScenehk(LPDIRECT3DDEVICE9 pDevice)
{
	return D3D9Hook::getInstance()->initHookCallback(pDevice);
}
