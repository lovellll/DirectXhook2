#include "Menu.h"

HWND Menu::m_hTargetWindow = nullptr;
LONG_PTR Menu::m_ulOldWndProc = NULL;
Menu* Menu::instance = nullptr;
bool Menu::bWasInitialized = false;

void Menu::initialize(LPDIRECT3DDEVICE9 pDevice)
{
	D3DDEVICE_CREATION_PARAMETERS params;
	pDevice->GetCreationParameters(&params);
	m_hTargetWindow = params.hFocusWindow;
	//use this to hook Window Procedures
	m_ulOldWndProc = SetWindowLongPtr(m_hTargetWindow, GWLP_WNDPROC, (LONG_PTR)WndProc);
}

LRESULT __stdcall Menu::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam) && GetKeyState(VK_DELETE) == 1)
		return true;

	return CallWindowProc((WNDPROC)getInstance()->m_ulOldWndProc, hWnd, msg, wParam, lParam);
}



