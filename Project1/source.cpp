#include "source.h"

using namespace std;

extern void memory();
extern void func();
extern void autoClick();
extern void findPlayer();
extern void findMob();

extern HANDLE hProc, hThread;
extern int Hour, Min;
extern string sHpStatus, sTravaStatus, sDanoStatus, sPlayerCheck, sAutoClick;
extern void* pLogs, *pSkill;

bool bConsoleUpdate;
int updateTime;
string sGameStatus;

FILE* f;

BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
{
	if (dwCtrlType == CTRL_CLOSE_EVENT)
	{
		TerminateProcess(hProc, 0);
		CloseHandle(hProc);
		CloseHandle(hThread);

		MessageBoxA(0, "Developed by: dwSize \n\nContato via Discord: dwSize#3592", "Contato", MB_OK | MB_ICONINFORMATION);
	}
	return TRUE;
}

void menu() {
	Sleep(200);
	if (bConsoleUpdate || clock() - updateTime > 3000) {
		updateTime = clock();
		system("cls");

		printf("/------------------------------------------------------------------------------/\n");
		printf("Tempo de jogo -> %.2d:%.2d min | Developed by: dwSize\n", Hour / 60, Min);
		printf("/------------------------------------------------------------------------------/\n");
		cout << sGameStatus;

		//DWORD a = 1049092675;
		//printf("%d", dwSerialHD);
		//printf("%08X", pSkill);
		//printf("%08x", BaseZF_07);
		/*cout << "\n\nPid: " << Pid;
		cout << "\nhProc: " << hProc;
		cout << "\n\nTID: " << GetThread;
		cout << "\nhThread: " << hThread;*/

		if (sGameStatus == "Game Status -> Funcoes Liberadas!") {
			cout << "\n/------------------------------------------------------------------------------/\n";
			cout << "[ALT + 1] -> Trava Hp [ " << sHpStatus << " ]\n";
			cout << "[ALT + 2] -> Trava Mp/Stm [ " << sTravaStatus << " ]\n";
			cout << "[ALT + 3] -> Dano 2x [ " << sDanoStatus << " ]\n";
			cout << "[ALT + 4] -> Core Magico\n";
			//cout << "[ALT + 5] -> Auto Click [ " << sAutoClick << " ]";
			
			cout << sPlayerCheck;
		}

		bConsoleUpdate = false;
	}
}

void logx() {
	while (true) {
		Sleep(50);
		
		SYSTEMTIME sLocalTime;
		GetLocalTime(&sLocalTime);

		if (f != NULL) {
			int packetCode = readMem(hProc, (DWORD)pLogs + 0x28, 4);
			int dwSkillCode = readMem(hProc, (DWORD)pLogs + 0x2c, 4);
			int packetWParam = readMem(hProc, (DWORD)pLogs + 0x30, 4);

			if (packetCode > 0) {
				if (fprintf(f, "%.2d:%.2d:%.2d : Packet: 0x%08X - WParam: 0x%08X", sLocalTime.wHour, sLocalTime.wMinute, sLocalTime.wSecond, packetCode, packetWParam)) {
					if (packetCode == 0x50322010)
						fprintf(f, " - SkillCode: 0x%X", dwSkillCode);

					fprintf(f, "\n");

					for (int i = 0; i < 3; i++)
						writeMem(hProc, (DWORD)pLogs + 0x28 + (i*4), (byte*)"\x0", 4);
				}
			}
		}
		else {
			char szPath[MAX_PATH];
			char* log = (char*)"logs.txt";

			GetFullPathName(log, MAX_PATH, szPath, 0);

			f = fopen(szPath, "a");
			fprintf(f, "%d:%d:%d : > **** Starting Service **** - ( %.2d/%.2d )\n\n", sLocalTime.wHour, sLocalTime.wMinute, sLocalTime.wSecond, sLocalTime.wMonth, sLocalTime.wDay);
		}
	}
}

bool bRegister() {
	DWORD dwSerialHD = 0; //variavel do serial do pc do usuario
	DWORD dwSerialHD_Client[] = { -530791459, -58256848, 1049092675, -522589021, 318002682, -118114608, 1022485426, 1083714008 }; //vetor de cadastro de serial
	
	if (!GetVolumeInformationA((char*)"C:\\", NULL, 0, &dwSerialHD, NULL, NULL, NULL, 0)) //pega o serial do disco C:
		return FALSE;

	for (int i = 0; i < 8; i++) { //até o tamanho do vetor
		if (to_string(dwSerialHD_Client[i]) == to_string(dwSerialHD)) //se o serial do vetor for igual ao serial do usuario, então..
			return TRUE;
	}
	return FALSE;
}

int main() {
	bConsoleUpdate = true;

	updateTime = clock();

	if (bRegister()) { //se o retorno da função for TRUE, então..
		SetConsoleCtrlHandler((PHANDLER_ROUTINE)HandlerRoutine, TRUE);

		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)logx, 0, 0, 0);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)permission, 0, 0, 0);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)memory, 0, 0, 0);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)func, 0, 0, 0);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)autoClick, 0, 0, 0);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)findPlayer, 0, 0, 0);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)findMob, 0, 0, 0);

		while (true)
			menu();
	}

	return 0;
}
