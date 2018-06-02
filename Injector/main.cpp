#include <iostream>
#include <windows.h>
#include <string>
#include <tlhelp32.h>

DWORD getWindowPID(LPCWSTR windowName)
{
	HWND hWindow = FindWindowW(NULL, windowName);
	DWORD PID;
	GetWindowThreadProcessId(hWindow, &PID);
	CloseHandle(hWindow);
	return PID;
}

DWORD getProcPID(std::wstring processName)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			std::wstring binPath = entry.szExeFile;
			if (binPath.find(processName) != std::wstring::npos)
			{
				printf("PID is %d\n", entry.th32ProcessID);
				CloseHandle(snapshot);
				return entry.th32ProcessID;
			}
		}
	}
	CloseHandle(snapshot);
	return NULL;
}

HANDLE getHandle(DWORD PID)
{
	//HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, false, PID);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if (hProcess == INVALID_HANDLE_VALUE || hProcess == NULL)
	{
		printf("Failed to open PID %d, error code %d\n", PID, GetLastError());
	}
	return hProcess;
}

template<typename T>
T readMemory(HANDLE hProcess, LPVOID adr)
{
	T val;
	DWORD oldProtect;
	VirtualProtectEx(hProcess, adr, sizeof(T), PAGE_READWRITE, &oldProtect);
	ReadProcessMemory(hProcess, adr, &val, sizeof(T), NULL);
	VirtualProtectEx(hProcess, adr, sizeof(T), oldProtect, NULL);
	return val;
}

template<typename T>
void writeMemory(HANDLE hProcess, LPVOID adr, T val)
{
	DWORD oldProtect;
	VirtualProtectEx(hProcess, adr, sizeof(T), PAGE_READWRITE, &oldProtect);
	WriteProcessMemory(hProcess, adr, &val, sizeof(T), NULL);
	VirtualProtectEx(hProcess, adr, sizeof(T), oldProtect, NULL);
}

DWORD getBase(HANDLE hProcess)
{
	DWORD newBase;

	//get address of kernel32.dll
	HMODULE k32 = GetModuleHandle(L"kernel32.dll");
	//get address of GetModuleHandle()
	LPVOID funcAdr = GetProcAddress(k32, "GetModuleHandleA");
	if (!funcAdr)
		funcAdr = GetProcAddress(k32, "GetModuleHandleW");

	//create the thread
	HANDLE thread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)funcAdr, NULL, NULL, NULL);
	WaitForSingleObject(thread, INFINITE);
	GetExitCodeThread(thread, &newBase);
	return newBase;

	CloseHandle(thread);
}

void inject(HANDLE hProcess)
{
	//wirte dll name to target memory
	wchar_t* dllName = L"DirectXhook2.dll";
	int namelen = wcslen(dllName) + 1;
	LPVOID remoteStringAdr = VirtualAllocEx(hProcess, NULL, namelen * 2, MEM_COMMIT, PAGE_EXECUTE);
	WriteProcessMemory(hProcess, remoteStringAdr, dllName, namelen * 2, NULL);

	//get the address of LoadLibraryW()
	HMODULE k32 = GetModuleHandleA("kernel32.dll");
	LPVOID funcAdr = GetProcAddress(k32, "LoadLibraryW");

	//create a thread
	HANDLE thread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)funcAdr, remoteStringAdr, NULL, NULL);
	WaitForSingleObject(thread, INFINITE);
	if (thread)
		printf("Inject successfully!");
	CloseHandle(thread);
}

int main()
{
	wchar_t* binName = L"Chapter8_Direct3DApplication.exe";
	DWORD procPID = getProcPID(binName);
	HANDLE hProcess = getHandle(procPID);
	DWORD base = getBase(hProcess);
	std::cout << "Base is: " << std::hex << base << std::endl;
	inject(hProcess);

	CloseHandle(hProcess);
	//getchar();
	return 0;
}

