#include "api.h"

DWORD getPid()
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (hSnapShot != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(hSnapShot, &pe32))
		{
			do {
				std::string full_name(pe32.szExeFile);
				if (full_name.find(".exe ") != -1)
				{
					CloseHandle(hSnapShot);
					return pe32.th32ProcessID;
				}
			} while (Process32Next(hSnapShot, &pe32));
		}
	}
	CloseHandle(hSnapShot);
	return 0;
}

DWORD getThreadID(DWORD dwPid) {
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwPid);
	DWORD Tid = 0;

	if (hSnap != INVALID_HANDLE_VALUE && dwPid > 0) {
		THREADENTRY32 te32;
		te32.dwSize = sizeof(THREADENTRY32);

		if (Thread32First(hSnap, &te32)) {
			do {
				if (te32.th32OwnerProcessID == dwPid)
				{
					Tid = te32.th32ThreadID;
					break;
				}
			} while (Thread32Next(hSnap, &te32));
		}
	}
	CloseHandle(hSnap);
	return Tid;
}

HANDLE openProc(DWORD dwPid) {
	return OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, dwPid);
}

void hookFunc(HANDLE hProc, DWORD opcode, DWORD dst, DWORD src, byte* Value, unsigned int size)
{
	int Hook = DWORD(dst) - DWORD(src) - 5;

	WriteProcessMemory(hProc, (void*)src, &opcode, 1, NULL);
	WriteProcessMemory(hProc, (void*)(src + 1), &Hook, sizeof(Hook), NULL);

	if (Value != NULL)
		WriteProcessMemory(hProc, (void*)(src + 5), Value, size, NULL);
}

void HookFunc_2(HANDLE hProc, DWORD MyFunc, DWORD ToHook, byte* Value, unsigned int size)
{
	int Opcode = 0x850F;
	int Hook = DWORD(MyFunc) - DWORD(ToHook) - 6;

	WriteProcessMemory(hProc, (void*)ToHook, &Opcode, 2, NULL);
	WriteProcessMemory(hProc, (void*)(ToHook + 2), &Hook, sizeof(Hook), NULL);

	if (Value != NULL)
		WriteProcessMemory(hProc, (void*)(ToHook + 6), Value, size, NULL);
}

void HookFunc_3(HANDLE hProc, DWORD MyFunc, DWORD ToHook)
{
	int Hook = DWORD(MyFunc) - DWORD(ToHook) - 5;

	WriteProcessMemory(hProc, (void*)(ToHook + 1), &Hook, sizeof(Hook), NULL);
}

int aobScan(HANDLE hProc, byte* value, int size) {
	MEMORY_BASIC_INFORMATION mbi = { 0 };
	int base = 0x07000000;
	void* pData = malloc(0x7FFFFF);

	do {
		base += mbi.RegionSize;
		VirtualQueryEx(hProc, (void*)base, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
	} while (mbi.RegionSize != size && base < 0x0C000000);

	if (mbi.RegionSize > size || base > 0x0BF00000)
		return 0;

	ReadProcessMemory(hProc, (void*)base, pData, mbi.RegionSize, NULL);

	if (value > 0)
		for (int i = 0; i < mbi.RegionSize; i++)
			if (*(DWORD*)(DWORD(pData) + i) == *(DWORD*)value)
				return base + i;

	return base;
}

int copy_paste(HANDLE hProc, DWORD addrCopy, void* addrPaste, int MAX_REGION) {
	//Aloca um endereço no processo do game
	if (addrPaste == 0)
		addrPaste = VirtualAllocEx(hProc, 0, MAX_REGION, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	//laço para ler e escrever
	for (int cnt = 0; cnt < MAX_REGION; cnt++) {
		int value = readMem(hProc, addrCopy + cnt, 1); //Lê 1 byte do endereço

		write(hProc, (DWORD)addrPaste + cnt, value, 1); //Escreve o 1 byte lido no endereço alocado
	}

	return (DWORD)addrPaste;
}

void writeMem(HANDLE hProc, DWORD addr, byte* value, int size) {
	WriteProcessMemory(hProc, (void*)addr, value, size, NULL);
}

void write(HANDLE hProc, DWORD addr, int value, int size) {
	WriteProcessMemory(hProc, (void*)addr, &value, size, NULL);
}

DWORD readMem(HANDLE hProc, DWORD addr, int size) {
	DWORD retorno = 0;
	ReadProcessMemory(hProc, (void*)addr, &retorno, size, NULL);
	return retorno;
}

void SendMsg(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	SendMessageA(hWnd, Msg, wParam, lParam);
}