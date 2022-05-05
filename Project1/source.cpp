#include "source.h"

using namespace std;

extern void memory();
extern void func();

extern HANDLE hProc, hThread;
extern int Hour, Min;
extern string sHpStatus, sTravaStatus, sDanoStatus;
extern void* pLogs;

bool bConsoleUpdate;
int updateTime;
string sGameStatus;

FILE* f;

BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
{
	if (dwCtrlType == CTRL_CLOSE_EVENT)
	{
		fprintf(f, "\n");
		fclose(f);

		TerminateProcess(hProc, 0);
		CloseHandle(hProc);
		CloseHandle(hThread);

		MessageBoxA(0, "Desenvolvido por: dwSize \n\nContato via Discord: dwSize#3592", "Contato", MB_OK | MB_ICONINFORMATION);
	}
	return TRUE;
}

void menu() {
	Sleep(200);
	if (bConsoleUpdate || clock() - updateTime > 3000) {
		updateTime = clock();
		system("cls");

		printf("/------------------------------------------------------------------------------/\n");
		printf("Tempo de jogo -> %.2d:%.2d min | Desenvolvido por: dwSize\n", Hour / 60, Min);
		printf("/------------------------------------------------------------------------------/\n");
		cout << sGameStatus << endl << endl;

		//DWORD a = 1049092675;
		//printf("%d", dwSerialHD);
		//printf("%08X", pOndaNegra);
		//printf("%08x", BaseZF_07);
		/*cout << "\n\nPid: " << Pid;
		cout << "\nhProc: " << hProc;
		cout << "\n\nTID: " << GetThread;
		cout << "\nhThread: " << hThread;*/

		//cout << "/------------------------------------------------------------------------------/\n";

		if (sGameStatus == "Game Status -> Funcoes Liberadas!") {
			cout << "[ALT + 1] -> Trava Hp [ " << sHpStatus << " ]\n";
			cout << "[ALT + 2] -> Trava Mp/Stm [ " << sTravaStatus << " ]\n";
			cout << "[ALT + 3] -> Dano 2x [ " << sDanoStatus << " ]";
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

					writeMem(hProc, (DWORD)pLogs + 0x28, (byte*)"\x0", 4);
					writeMem(hProc, (DWORD)pLogs + 0x2c, (byte*)"\x0", 4);
					writeMem(hProc, (DWORD)pLogs + 0x30, (byte*)"\x0", 4);
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

int main() {
	bConsoleUpdate = true;

	DWORD dwSerialHD = 0;
	DWORD dwSerialHD_Client[] = { -530791459, 1049092675 }; //vetor de cadastro de serial
	string sSerialHD_Client[2]; //vetor do serial convertido
	bool bFind = false; //variavel de controle

	updateTime = clock();

	if (!GetVolumeInformationA((char*)"C:\\", NULL, 0, &dwSerialHD, NULL, NULL, NULL, 0)) //pega o serial do disco C:
		return 0;

	for (int i = 0; i < sizeof(dwSerialHD_Client); i++) { //até o tamanho do vetor
		if (to_string(dwSerialHD_Client[i]) == to_string(dwSerialHD)) { //se o serial do vetor for igual ao serial do usuario, então..
			bFind = true; break;
		}
	}

	if (bFind) {
		SetConsoleCtrlHandler((PHANDLER_ROUTINE)HandlerRoutine, TRUE);

		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)logx, 0, 0, 0);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)permission, 0, 0, 0);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)memory, 0, 0, 0);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)func, 0, 0, 0);

		while (true)
			menu();
	}

	return 0;
}
