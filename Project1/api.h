#include <Windows.h>
#include <TlHelp32.h>
#include <string>

DWORD getPid();
DWORD getThreadID(DWORD dwPid);
HANDLE openProc(DWORD dwPid);
void hookFunc(HANDLE hProc, DWORD opcode, DWORD dst, DWORD src, byte* Value = 0, unsigned int size = 0);
void HookFunc_2(HANDLE hProc, DWORD MyFunc, DWORD ToHook, byte* Value = 0, unsigned int size = 0);
int aobScan(HANDLE hProc, byte* value = 0, int size = 0);
int copy_paste(HANDLE hProc, DWORD addrCopy, void* addrPaste, int MAX_REGION);
void writeMem(HANDLE hProc, DWORD addr, byte* value, int size);
void write(HANDLE hProc, DWORD addr, int value, int size);
DWORD readMem(HANDLE hProc, DWORD addr, int size);
void SendMsg(HWND hWnd, UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0);