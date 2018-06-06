#pragma once
#include <Windows.h>
#include "../imgui/imgui.h"
#include "../imgui/examples/directx9_example/imgui_impl_dx9.h"
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

class Menu
{
public:
	Menu() {
		ImGui::CreateContext();
	};
	~Menu() {
		ImGui::DestroyContext();
	};

	static Menu* getInstance()
	{
		if (!Menu::instance)
			Menu::instance = new Menu;
		return Menu::instance;
	}

	static void deldeteInstance()
	{
		if (Menu::instance)
		{
			delete Menu::instance;
			Menu::instance = nullptr;
		}
	}

	void initialize(LPDIRECT3DDEVICE9 pDevice);
	static bool bWasInitialized;

private:
	static Menu* instance;
	static HWND m_hTargetWindow;
	static LONG_PTR m_ulOldWndProc;

	static LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};

