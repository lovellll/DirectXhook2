#include "Memory.h"
#include "offsets.h"

DWORD Memory::getPointAddress(DWORD weapon_baseOffset, const std::vector<DWORD> &offsets)
{
	if (baseAddr == NULL)  //baseAddr!
		return NULL;

	DWORD pointAddress = readMemory<DWORD>((LPVOID)(baseAddr + weapon_baseOffset)); //get base pointer address
	int level = offsets.size();
	if (offsets.size() != 0)
	{
		for (int i = 0; i < level; ++i)
		{
			if (i == level - 1)
			{
				return(pointAddress + offsets[i]);   //if it gets to last level, plus the offet and 
													 //will not take it as a pointer and go into it
			}
			pointAddress = readMemory<DWORD>((LPVOID)(pointAddress + offsets[i]));
		}
	}
	return pointAddress;
}

DWORD Memory::getVF(DWORD classInst, DWORD funcIndex)
{
	DWORD VFTable = readMemory<DWORD>((LPVOID)classInst);
	DWORD VFaddress = VFTable + funcIndex * sizeof(DWORD);
	return readMemory<DWORD>((LPVOID)VFaddress);
}
