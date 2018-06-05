#include <Windows.h>
#include <iostream>
#include "offsets.h"
#include "D3D9Hook.h"
#include "DebugConsole.h"


DWORD WINAPI ATTACHED(LPVOID lpParam);
DWORD WINAPI DETACHED(LPVOID lpParam);

BOOL APIENTRY DllMain(HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		auto thread = CreateThread(NULL, 0, &ATTACHED, NULL, 0, NULL);
		CloseHandle(thread);
		break;
	}
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		auto thread1 = CreateThread(NULL, 0, &DETACHED, NULL, 0, NULL);
		CloseHandle(thread1);

		break;
	}
	return TRUE;
}

DWORD WINAPI ATTACHED(LPVOID lpParam)
{
#ifdef _DEBUG
	DebugConsole::AttachConsole();
	DebugConsole::ConsolePrint("DLL injected!\n");
#endif
	baseAddr = (DWORD)GetModuleHandle(NULL);
	D3D9Hook::getInstance()->initialize();
	return 1;
}

DWORD WINAPI DETACHED(LPVOID lpParam)
{
	return 1;
}
