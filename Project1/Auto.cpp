#include "Auto.h"

extern HANDLE hProc;
extern HWND hWnd;
extern void SendMsg(HWND hWnd, UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0);
extern void write(HANDLE hProc, DWORD addr, int value, int size);
extern bool bAutoClick;
extern std::string sPlayerCheck, sAutoClick;

bool bGirarTela;

void autoClick() {
	while (true) {
		Sleep(100);

		if (bAutoClick) {
			if (bGirarTela)
				SendMsg(hWnd, WM_KEYDOWN, VK_LEFT);

			srand(time(NULL));
			int time = rand() % 700 + 400;

			SendMsg(hWnd, WM_RBUTTONDOWN);
			Sleep(100);
			SendMsg(hWnd, WM_RBUTTONUP);
			Sleep(time);
		}
	}
}