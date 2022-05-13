#include "func.h"

extern HANDLE hProc;
extern string sGameStatus;
extern bool bPatchActive, bConsoleUpdate;
extern void* pSkill;

bool bGetTime;
int Time, Time2, Min, Hour;

bool bHp, bTrava, bDano;
string sHpStatus, sTravaStatus, sDanoStatus, sPlayerCheck;

void func() {
	int pUserData;

	while (true) {
		Sleep(200);
		if (bPatchActive) {

			pUserData = readMem(hProc, 0x033DED30, 4); //pointer

			if (readMem(hProc, pUserData - 0x1D0, 4)) {
				getTime();

				sGameStatus = "Game Status -> Funcoes Liberadas!";

				hotkey();
				olhoMagic();
				active_func();
			}
			else {
				sGameStatus = "Game Status -> Aguardando login ingame..";

				sHpStatus = "Off", sTravaStatus = "Off", sDanoStatus = "Off", sPlayerCheck = "Alerta -> Nenhum jogador avistado ao redor!";
				bHp = false, bTrava = false, bDano = false, bGetTime = false;
			}
		}
	}
}

void getTime() {
	if (!bGetTime) {
		Time = GetTickCount();
		Time2 = GetTickCount();

		bGetTime = true;
	}

	Min = ((GetTickCount() - Time) / 1000) / 60;

	if (Min == 60)
		Time2 = GetTickCount();

	Hour = ((GetTickCount() - Time2) / 1000) / 60;
}

void hotkey() {
	if (GetAsyncKeyState(VK_MENU) & 0x8000) {
		if (GetAsyncKeyState(0x31) < 0) {
			if (!bHp) {
				sHpStatus = "On";
				bHp = true, bConsoleUpdate = true;
				Beep(500, 500);
			}
			else {
				sHpStatus = "Off";
				bHp = false, bConsoleUpdate = true;
				Beep(500, 500);
			}
			Sleep(200);
		}

		if (GetAsyncKeyState(0x32) & 0x8000) {
			if (!bTrava) {
				sTravaStatus = "On";
				bTrava = true, bConsoleUpdate = true;
				Beep(500, 500);
			}
			else {
				sTravaStatus = "Off";
				bTrava = false, bConsoleUpdate = true;
			}
			Sleep(200);
		}

		if (GetAsyncKeyState(0x33) & 0x8000) {
			writeMem(hProc, 0x03396E0C, (byte*)"\x08", 1);

			Beep(500, 500);
			Sleep(200);
		}

		if (GetAsyncKeyState(0x34) & 0x8000) {
			if (!bDano) {
				sDanoStatus = "On";
				bDano = true, bConsoleUpdate = true;
				Beep(500, 500);
			}
			else {
				sDanoStatus = "Off";
				bDano = false, bConsoleUpdate = true;
				Beep(500, 500);
			}
			Sleep(200);
		}
	}
}

void olhoMagic() {
	int chrOtherPlayer = 0x0B0A218, somaOtherPlayer = 0x4CF0, pMotionInfo = 0, lpCurPlayer, x, y, z;
	sPlayerCheck = "";

	for (int i = 0; i < 1024; i++) {
		pMotionInfo = readMem(hProc, chrOtherPlayer + 0x4794, 4);

		//Flag - smCHAR_STATE_USER - CHRMOTION_STATE_DEAD
		if (readMem(hProc, chrOtherPlayer + 0x33C, 4) > 0 && readMem(hProc, chrOtherPlayer + 0x35C, 4) > 0 && readMem(hProc, chrOtherPlayer + 0x1D4, 4) > 0 && readMem(hProc, chrOtherPlayer + 0x39C4, 4) == 0x80 && readMem(hProc, pMotionInfo, 4) != 0x120) {
			
			lpCurPlayer = readMem(hProc, 0xAFE60C, 4);
			x = readMem(hProc, chrOtherPlayer + 0x1D8, 4) - readMem(hProc, lpCurPlayer + 0x1E8, 4);
			y = readMem(hProc, chrOtherPlayer + 0x1DC, 4) - readMem(hProc, lpCurPlayer + 0x1EC, 4);
			z = readMem(hProc, chrOtherPlayer + 0x1E0, 4) - readMem(hProc, lpCurPlayer + 0x1F0, 4);

			if (abs(x) < 200000 && abs(z) < 200000) {
				if (sPlayerCheck == "") {
					sPlayerCheck = "Alerta -> Player proximo avistado ao redor!";
					bConsoleUpdate = true;
				}
			}
			else
				sPlayerCheck = "";
		}

		if (chrOtherPlayer < 0x1E46218)
			chrOtherPlayer += somaOtherPlayer;
	}
}

void active_func() {
	if (bHp)
		writeMem(hProc, 0x0041FB5F, (byte*)"\x90\x90", 2);
	else
		writeMem(hProc, 0x0041FB5F, (byte*)"\x2b\xc5", 2);

	if (bTrava) {
		writeMem(hProc, 0x004E1C6D, (byte*)"\x1", 1);
		writeMem(hProc, 0x004E1C6D + 0x45, (byte*)"\x1", 1);
	}
	else {
		writeMem(hProc, 0x004E1C6D, (byte*)"\x4", 1);
		writeMem(hProc, 0x004E1C6D + 0x45, (byte*)"\x4", 1);
	}

	if (bDano)
		write(hProc, (DWORD)pSkill + 0x82, 1, 1);
	else
		write(hProc, (DWORD)pSkill + 0x82, 0, 1);
}
