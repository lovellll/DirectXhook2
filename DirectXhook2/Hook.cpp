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

//-----------------------those are testing DX hooks---------------//
void enableLighthackDirectional(LPDIRECT3DDEVICE9 pDevice)
{
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	light.Direction = D3DXVECTOR3(-1.0f, -0.5f, -1.0f);

	pDevice->SetLight(0, &light);
	pDevice->LightEnable(0, true);
}

void enableLightHackAmbient(LPDIRECT3DDEVICE9 pDevice)
{
	pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(100, 100, 100));
}
//-----------------------those are testing DX hooks---------------//