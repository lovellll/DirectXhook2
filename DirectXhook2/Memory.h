#pragma once
#include <Windows.h>
#include <vector>



namespace Memory
{
	DWORD getPointAddress(DWORD weapon_baseOffset, const std::vector<DWORD>& offsets);
	DWORD getVF(DWORD classInst, DWORD funcIndex);

	template<typename T>
	void writeToPoint(DWORD baseoffset, std::vector<DWORD> offsets, T val)
	{
		DWORD address = getPointAddress(baseoffset, offsets);
		writeMemory<T>((LPVOID)address, val, TRUE);
	}

	template<typename T>
	T readMemory(LPVOID adr)
	{
		T val;
		DWORD oldProtect;
		VirtualProtect(adr, sizeof(T), PAGE_READWRITE, &oldProtect);
		val = *((T*)adr);
		VirtualProtect(adr, sizeof(T), oldProtect, NULL);
		return val;
	}

	template<typename T>
	void writeMemory(LPVOID adr, T val, bool isRestProct = TRUE)
	{
		DWORD oldProtect;
		VirtualProtect(adr, sizeof(T), PAGE_EXECUTE_READWRITE, &oldProtect);
		*((T*)adr) = val;
		if (isRestProct == TRUE)
			VirtualProtect(adr, sizeof(T), oldProtect, NULL);
	}


	template<typename T>
	T* pointMemory(LPVOID adr)
	{
		return((T*)adr);
	}
}