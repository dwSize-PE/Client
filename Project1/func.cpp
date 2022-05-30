#include "func.h"

extern HANDLE hProc;
extern string sGameStatus;
extern bool bPatchActive, bConsoleUpdate, bGirarTela;
extern void* pSkill, * pMob, * pMob2, * pDamage, *pMinMaxDamage, * pfield, * pRank;

bool bGetTime;
int Time, Time2, Min, Hour;

bool bHp, bTrava, bDano, bAutoClick, bTelep, bSendTransDamage, bSendRangeDamage;
string sHpStatus, sTravaStatus, sDanoStatus, sPlayerCheck, sAutoClick, sTransDamage, sRangeDamage;

int pUserData;

void func() {
	while (true) {
		Sleep(200);
		if (bPatchActive) {

			pUserData = readMem(hProc, 0x033DED30, 4); //pointer

			if (readMem(hProc, pUserData - 0x1D0, 4)) {
				getTime();

				sGameStatus = "Game Status -> Funcoes Liberadas!";

				hotkey();
				active_func();
			}
			else {
				sGameStatus = "Game Status -> Aguardando login ingame..";

				sHpStatus = "Off", sTravaStatus = "Off", sDanoStatus = "Off", sAutoClick = "Off", sTransDamage = "0ff", sRangeDamage = "Off";
				bHp = false, bTrava = false, bDano = false, bAutoClick = false, bSendTransDamage = false, bSendRangeDamage = false, bTelep = false, bGirarTela = false, bGetTime = false;
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
		Time = GetTickCount();

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

		if (GetAsyncKeyState(0x34) & 0x8000) {
			writeMem(hProc, 0x03396E0C, (byte*)"\x08", 1);

			Beep(500, 500);
			Sleep(200);
		}

		if (GetAsyncKeyState(0x35) & 0x8000) {
			if (!bAutoClick) {
				Beep(500, 500);

				if(!bTelep)
					if (MessageBoxA(0, "Deseja ativar o Teleporte Automatico?", "", MB_ICONQUESTION | MB_YESNO) == IDYES)
						bTelep = true;

				if (MessageBox(0, "Deseja girar a tela?", "", MB_ICONQUESTION | MB_YESNO) == IDYES)
					bGirarTela = true;

				sAutoClick = "On";
				bAutoClick = true, bConsoleUpdate = true;
			}
			else
			{
				sAutoClick = "Off";
				bAutoClick = false, bTelep = false, bGirarTela = false, bConsoleUpdate = true;
				Beep(500, 500);
			}
			Sleep(200);
		}

		if (GetAsyncKeyState(0x54) & 0x8000) { //T
			write(hProc, (DWORD)pRank, 1, 4);

			Beep(500, 500);
			Sleep(200);
		}

		if (GetAsyncKeyState(0x36) & 0x8000) {
			if (!bSendTransDamage) {
				if(!bTelep)
					if (MessageBoxA(0, "Deseja ativar o Teleporte Automatico?", "", MB_ICONQUESTION | MB_YESNO) == IDYES)
						bTelep = true;

				sTransDamage = "On";
				Beep(500, 500);
				bSendTransDamage = true, bConsoleUpdate = true;
			}
			else {
				sTransDamage = "Off";
				Beep(500, 500);
				bSendTransDamage = false, bTelep = false, bConsoleUpdate = true;
			}
			Sleep(200);
		}

		if (GetAsyncKeyState(0x37) & 0x8000) {
			if (!bSendRangeDamage) {
				if (MessageBoxA(0, "Meteoro ou Bola de Fogo?", "", MB_ICONQUESTION | MB_YESNO) == IDYES) {
					write(hProc, (DWORD)pDamage + 0x3E, 0x15, 1);
					write(hProc, (DWORD)pDamage + 0x45, 0x05, 1);

					write(hProc, (DWORD)pMinMaxDamage, 1457, 2); //Min
					write(hProc, (DWORD)pMinMaxDamage + 0x2, 1469, 2); //Max

					write(hProc, (DWORD)pDamage + 0x32, 0x4D, 1);
				}
				else {
					write(hProc, (DWORD)pDamage + 0x3E, 0x90, 1);
					write(hProc, (DWORD)pDamage + 0x45, 0x80, 1);

					write(hProc, (DWORD)pMinMaxDamage, readMem(hProc, 0x033B2924, 2), 2); //Min
					write(hProc, (DWORD)pMinMaxDamage + 0x2, readMem(hProc, 0x033B2926, 2), 2); //Max

					write(hProc, (DWORD)pDamage + 0x32, 0x1D, 1);
				}

				if (!bTelep)
					if (MessageBoxA(0, "Deseja ativar o Teleporte Automatico?", "", MB_ICONQUESTION | MB_YESNO) == IDYES)
						bTelep = true;

				sRangeDamage = "On";
				Beep(500, 500);
				bSendRangeDamage = true, bConsoleUpdate = true;
			}
			else {
				sRangeDamage = "Off";
				Beep(500, 500);
				bSendRangeDamage = false, bTelep = false, bConsoleUpdate = true;
			}
			Sleep(200);
		}
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

	if (bDano) {
		write(hProc, (DWORD)pSkill + 0x97, 1, 1);
		write(hProc, (DWORD)pSkill + 0x83, 1, 1);
	}
	else {
		write(hProc, (DWORD)pSkill + 0x97, 0, 1);
		write(hProc, (DWORD)pSkill + 0x83, 1, 1);
	}
}

void findPlayer() {
	bool bTimeTelep = false, bLol = false;
	int update_time = 0;

	while (true) {
		Sleep(100);
		if (bPatchActive) {
			if (bTimeTelep)
				if (clock() - update_time > 30000)
					bLol = true;

			sPlayerCheck = "";

			int chrOtherPlayer = 0x0B0A218, somaOtherPlayer = 0x4CF0, pMotionInfo = 0, lpCurPlayer, x, y, z;

			for (int i = 0; i < 1024; i++) {
				pMotionInfo = readMem(hProc, chrOtherPlayer + 0x4794, 4);

				//Flag - PartyFlag - smCHAR_STATE_USER - CHRMOTION_STATE_DEAD
				if (readMem(hProc, chrOtherPlayer + 0x35C, 4) > 0 && readMem(hProc, chrOtherPlayer + 0x4898, 4) == 0 && readMem(hProc, chrOtherPlayer + 0x39C4, 4) == 0x80 && readMem(hProc, pMotionInfo, 4) != 0x120) {

					lpCurPlayer = readMem(hProc, 0xAFE60C, 4);
					x = readMem(hProc, chrOtherPlayer + 0x1D8, 4) - readMem(hProc, lpCurPlayer + 0x1E8, 4);
					y = readMem(hProc, chrOtherPlayer + 0x1DC, 4) - readMem(hProc, lpCurPlayer + 0x1EC, 4);
					z = readMem(hProc, chrOtherPlayer + 0x1E0, 4) - readMem(hProc, lpCurPlayer + 0x1F0, 4);
					//printf("\n\n%08X", chrOtherPlayer);

					if (abs(x) < 136000 && abs(z) < 137000) {
						//printf("\n\n%08X", chrOtherPlayer);
						if (sPlayerCheck == "") {
							sPlayerCheck = "\n\nAlerta -> Player proximo avistado ao redor!";

							if (!bTimeTelep) {
								update_time = clock();
								bTimeTelep = true;
							}

							if (bTelep && bLol) {
								Beep(500, 500);

								sAutoClick = "Off";
								sTransDamage = "Off";
								sRangeDamage = "Off";
								bAutoClick = false, bSendTransDamage = false, bSendRangeDamage = false, bTelep = false, bGirarTela = false, bTimeTelep = false, bLol = false;
								write(hProc, (DWORD)pfield, 21, 4);
							}
							else
								write(hProc, (DWORD)pfield, 0, 4);
						}
					}
				}

				if (chrOtherPlayer < 0x1E46218)
					chrOtherPlayer += somaOtherPlayer;
			}
		}
	}
}

void findMob() {
	while (true) {
		Sleep(100);
		if (bPatchActive) {
			int chrOtherPlayer = 0x0B0A218, somaOtherPlayer = 0x4CF0, pMotionInfo = 0, lpCurPlayer, x, y, z;

			for (int i = 0; i < 1024; i++) {
				//Flag - smCHAR_STATE_ENEMY - Mob Life
				if (readMem(hProc, chrOtherPlayer, 4) > 0 && readMem(hProc, chrOtherPlayer + 0x39C4, 4) == 1 && readMem(hProc, chrOtherPlayer + 0x3A40, 2) > 0) {

					lpCurPlayer = readMem(hProc, 0xAFE60C, 4);
					x = readMem(hProc, chrOtherPlayer + 0x1D8, 4) - readMem(hProc, lpCurPlayer + 0x1E8, 4);
					y = readMem(hProc, chrOtherPlayer + 0x1DC, 4) - readMem(hProc, lpCurPlayer + 0x1EC, 4);
					z = readMem(hProc, chrOtherPlayer + 0x1E0, 4) - readMem(hProc, lpCurPlayer + 0x1F0, 4);

					if (abs(x) < 74000 && abs(z) < 50000) {
						//printf("\n\n%08X", chrOtherPlayer);
						if (bSendTransDamage)
							write(hProc, (DWORD)pMob2, chrOtherPlayer - 0x10, 4);

						if (bSendRangeDamage) {
							int Level = readMem(hProc, pUserData + 0xB8, 4);

							if (Level >= 17 && Level < 19 || Level >= 70 && Level < 72)
								writeMem(hProc, (DWORD)pDamage + 0x33, (byte*)"\x01", 1);
							else if (Level >= 19 && Level < 21 || Level >= 72 && Level < 74)
								writeMem(hProc, (DWORD)pDamage + 0x33, (byte*)"\x02", 1);
							else if (Level >= 21 && Level < 23 || Level >= 74 && Level < 76)
								writeMem(hProc, (DWORD)pDamage + 0x33, (byte*)"\x03", 1);
							else if (Level >= 23 && Level < 25 || Level >= 76 && Level < 78)
								writeMem(hProc, (DWORD)pDamage + 0x33, (byte*)"\x04", 1);
							else if (Level >= 25 && Level < 27 || Level >= 78 && Level < 80)
								writeMem(hProc, (DWORD)pDamage + 0x33, (byte*)"\x05", 1);
							else if (Level >= 27 && Level < 29 || Level >= 80 && Level < 82)
								writeMem(hProc, (DWORD)pDamage + 0x33, (byte*)"\x06", 1);
							else if (Level >= 29 && Level < 31 || Level >= 82 && Level < 84)
								writeMem(hProc, (DWORD)pDamage + 0x33, (byte*)"\x07", 1);
							else if (Level >= 31 && Level < 33 || Level >= 84 && Level < 86)
								writeMem(hProc, (DWORD)pDamage + 0x33, (byte*)"\x08", 1);
							else if (Level >= 33 && Level < 35 || Level >= 86 && Level < 88)
								writeMem(hProc, (DWORD)pDamage + 0x33, (byte*)"\x09", 1);
							else if (Level >= 35 && Level < 70 || Level >= 88)
								writeMem(hProc, (DWORD)pDamage + 0x33, (byte*)"\x0A", 1);

							write(hProc, (DWORD)pMob, chrOtherPlayer, 4);
						}

						Sleep(800);
					}
				}

				if (chrOtherPlayer < 0x1E46218)
					chrOtherPlayer += somaOtherPlayer;
			}
		}
	}
}
