#include <Windows.h>
#include "Structures.h"
#include "Helpers.h"
#include "PatternScanner.h"

int dmg = 0;
DWORD start = 0;
DWORD client = 0;
DWORD len = 7;
DWORD back = 0;

void GetDmg()
{
	if (!client)
		return;

	TNTClient* tntclient = **(TNTClient***)(client);

	char* dmgStr = tntclient->m_pNosRevCmdList->m_pStringList->Get(12);
	dmg = atoi(dmgStr);
}

__declspec(naked) void Hook()
{
	__asm
	{
		PUSHAD;
		PUSHFD;
		CALL GetDmg;
		POPFD;
		POPAD;
		MOV EAX, dmg;
		LEA EDX, DWORD PTR SS : [EBP - 0xC];
		JMP back;
	}
}

DWORD WINAPI DLLStart(LPVOID param)
{
	start = (DWORD)(FindPattern((BYTE*)0x400000, 0x2C0000, (BYTE*)"\xC6\x45\xFB\x00\x8A\x45\x08", "xxxxxxx")) + 0x26;
	client = *(DWORD*)(FindPattern((BYTE*)0x400000, 0x2C0000, (BYTE*)"\xA1\x00\x00\x00\x00\x8B\x00\xE8\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x8B\x00\x83\x78\x20\x00\x0F\x84\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x8B\x00", "x????xxx????x????xxxxxxxx????x????xx") + 1);
	back = start + len;

	if (back < 0x100)
		return 1;

	MakeJMP((BYTE*)start, (DWORD)&Hook, len);
	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		CreateThread(0, 0, DLLStart, 0, 0, 0);
		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

