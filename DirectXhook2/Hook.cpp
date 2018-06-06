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

DX_API resethk(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	return D3D9Hook::getInstance()->resetCallback(pDevice, pPresentationParameters);
}

DX_API drawIndexedPrimitivehk(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	return D3D9Hook::getInstance()->drawIndexedPrimitiveCallback(pDevice, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

