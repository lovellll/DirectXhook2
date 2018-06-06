#include "Menu.h"

HWND Menu::m_hTargetWindow = nullptr;
LONG_PTR Menu::m_ulOldWndProc = NULL;
Menu* Menu::instance = nullptr;
bool Menu::bWasInitialized = false;
bool Menu::bImguiWasInitialized = false;

void Menu::initialize(LPDIRECT3DDEVICE9 pDevice)
{
	D3DDEVICE_CREATION_PARAMETERS params;
	pDevice->GetCreationParameters(&params);
	m_hTargetWindow = params.hFocusWindow;
	//use this to hook Window Procedures
	m_ulOldWndProc = SetWindowLongPtr(m_hTargetWindow, GWLP_WNDPROC, (LONG_PTR)WndProc);
}

void Menu::imguiInitialize(LPDIRECT3DDEVICE9 pDevice)
{
	if (!bImguiWasInitialized)
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.DeltaTime = 1.0f / 60.0f;
		D3DDEVICE_CREATION_PARAMETERS d3dcp;
		ZeroMemory(&d3dcp, sizeof(d3dcp));
		pDevice->GetCreationParameters(&d3dcp);
		io.Fonts->AddFontDefault();
		ImGui_ImplDX9_Init(d3dcp.hFocusWindow, pDevice);
		bImguiWasInitialized = true;
	}
}

void Menu::renderFrame()
{
	ImGui_ImplDX9_NewFrame();
	
	drawMenu();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void Menu::drawMenu()
{

	//draw menu here
	ImGui::Begin("Another Window");

	ImGui::Text("Hello from another window!");
	if (ImGui::Button("Show text"))
		g_Options.text_enabled = !g_Options.text_enabled;
	if (ImGui::Button("Light!"))
		g_Options.light_enabled = !g_Options.light_enabled;
	if (ImGui::Button("Change Texture"))
		g_Options.texture_enabled = !g_Options.texture_enabled;

	ImGui::End();

	//ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
	//ImGui::ShowDemoWindow();
}

LRESULT __stdcall Menu::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam) && GetKeyState(VK_DELETE) == 1)
		return true;

	return CallWindowProc((WNDPROC)getInstance()->m_ulOldWndProc, hWnd, msg, wParam, lParam);
}



