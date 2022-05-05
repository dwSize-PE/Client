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

DWORD dwSerialHD = 0;

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
		//const time_t now = time(nullptr); // get the current time point
		//const tm calendar_time = *localtime(std::addressof(now));
		SYSTEMTIME sLocalTime;
		GetLocalTime(&sLocalTime);

		if (f != NULL) {
			int packetCode = readMem(hProc, (DWORD)pLogs + 0x28, 4);
			int dwSkillCode = readMem(hProc, (DWORD)pLogs + 0x2c, 4);
			int packetWParam = readMem(hProc, (DWORD)pLogs + 0x30, 4);

			if (packetCode > 0) {
				if (fprintf(f, "%.2d:%.2d:%.2d : Packet: 0x%08X - WParam: 0x%08X", sLocalTime.wHour, sLocalTime.wMinute, sLocalTime.wSecond/*calendar_time.tm_hour, calendar_time.tm_min, calendar_time.tm_sec*/, packetCode, packetWParam)) {
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
			fprintf(f, "%d:%d:%d : > **** Starting Service **** - ( %.2d/%.2d )\n\n", sLocalTime.wHour, sLocalTime.wMinute, sLocalTime.wSecond, sLocalTime.wMonth, sLocalTime.wDay/*calendar_time.tm_hour, calendar_time.tm_min, calendar_time.tm_sec, calendar_time.tm_mon + 1, calendar_time.tm_mday*/);
		}
	}
}

int main() {
	updateTime = clock();
	bConsoleUpdate = true;

	char szHD[4];

	szHD[0] = 'C';
	szHD[1] = ':';
	szHD[2] = '\\';
	szHD[3] = 0;

	if (!GetVolumeInformationA(szHD, NULL, 0, &dwSerialHD, NULL, NULL, NULL, 0))
		return 0;

	DWORD dwSerial = 1049092675;
	string Danilo = to_string(dwSerial);

	if (Danilo != to_string(dwSerialHD)) {
		system("pause");
		return 0;
	}

	SetConsoleCtrlHandler((PHANDLER_ROUTINE)HandlerRoutine, TRUE);

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)logx, 0, 0, 0); //
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)permission, 0, 0, 0);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)memory, 0, 0, 0);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)func, 0, 0, 0);

	while (true)
	{
		Sleep(200);
		menu();
	}

	return 0;
}
