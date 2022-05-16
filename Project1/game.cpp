#include "game.h"

using namespace std;

extern string sGameStatus;
extern bool bConsoleUpdate;

HANDLE hProc = 0, hThread = 0;
bool bActive;
extern bool bPatch, bPatchActive, bCopyGame;

extern FILE* f;

void permission() {
	DWORD dwPid, dwThreadId;
	bool bhProc = false, bhThreadId = false;

	while (true) {
		Sleep(200);
		dwPid = getPid();

		if (dwPid > 0) {
			if (!bhProc) {
				hProc = openProc(dwPid);

				if (hProc == 0)
				{
					sGameStatus = "Game Status -> Erro ao ter permissao de processo!";
					bConsoleUpdate = true, bhProc = true;
				}
				else
				{
					sGameStatus = "Game Status -> Permissao concedida!";
					bConsoleUpdate = true, bActive = true, bhProc = true;
				}
			}

			if (!bhThreadId) {
				dwThreadId = getThreadID(dwPid);

				hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, dwThreadId);

				if (hThread == 0)
				{
					sGameStatus = "Game Status -> Erro ao ter permissao de thread!";
					bConsoleUpdate = true, bActive = false, bhThreadId = true;
				}
				else
				{
					sGameStatus = "Game Status -> Permissao concedida!";
					bConsoleUpdate = true, bhThreadId = true;
				}
			}
		}
		else
		{
			sGameStatus = "Game Status -> Aguardando Priston Tale..";
			if (bhProc || bhThreadId) {
				CloseHandle(hProc);
				CloseHandle(hThread);
				hProc = 0, hThread = 0;

				fprintf(f, "\n");
				fclose(f);
				f = 0;

				bActive = false, bPatch = false, bPatchActive = false, bCopyGame = false, bhProc = false, bhThreadId = false;
			}
		}
	}
}