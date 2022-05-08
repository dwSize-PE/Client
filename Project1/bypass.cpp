#include "bypass.h"

using namespace std;

extern HANDLE hProc, hThread;
extern string sGameStatus;
extern bool bConsoleUpdate, bActive;

bool bPatch, bPatchActive, bCopyGame;
void* pOndaNegra, * pLogs;

void memory() {
	int hooksGame, hooksGame2, codeGame;//, BaseZF2_0A;

	while (true) {
		Sleep(200);
		if (!bPatch && bActive) {
			//Ponto de referencia para começar a alterar a memória
			hooksGame = readMem(hProc, 0x004E1C0C, 1);
			hooksGame2 = readMem(hProc, 0x00401000, 1);

			if (!bCopyGame) {
				if (hooksGame2 == 0xE9) {
					sGameStatus = "Game Status -> Aguardando resposta de game.exe..";
					bConsoleUpdate = true;

					SuspendThread(hThread);
					codeGame = copy_paste(hProc, 0x00401000, 0, 0x247FFF);
					ResumeThread(hThread);

					bCopyGame = true;
				}
			}
			else if (hooksGame == 0xE9) {
				//Retorna a Base Addr do endereço dinamico alocado pelo ZF
				int BaseZF_07 = aobScan(hProc, 0, 0x132000);
				//BaseZF2_0A = aobScan(hProc, 0, 0x124000);

				//Se encontrar..
				if (BaseZF_07 > 0) {
					sGameStatus = "Game Status -> Aguardando resposta de ZForce.dll..";
					bConsoleUpdate = true;

					//Suspende a thread para fazer as alterações afim de prevenir detecções antecipadas.
					SuspendThread(hThread);

					//copia todos os bytes da sessão ZForce
					int pAllocZF = copy_paste(hProc, BaseZF_07, 0, 0x1315D0);

					//copia todos os bytes da sessão Game.exe
					int pAllocGame = copy_paste(hProc, 0x00401000, 0, 0x247FFF);

					//espaço alocado dinâmico para pôr os bytes da sessão ZForce
					void* pbCheck = VirtualAllocEx(hProc, 0, 0xBC, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

					hookFunc(hProc, 0xE9, (DWORD)pbCheck, BaseZF_07 + 0x80BDC, (byte*)"\x90", 1); //Hook para o endereço alocado

					writeMem(hProc, (DWORD)pbCheck, (byte*)"\x81\xFE\xD0\x15\x13\x00" //cmp esi, 001315D0
						"\x74\x43"	//je ..
						"\x81\xFE\xFF\x7F\x24\x00" //cmp esi,00247FFF
						"\x74\x49" //je ..
						"\x81\xF9\x6D\x1C\x4E\x00" //cmp ecx,004E1C6D
						"\x74\x4F"
						"\x81\xF9\x15\x1C\x4E\x00" //cmp ecx,004E1C15
						"\x74\x55"
						"\x81\xF9\x8F\xF4\x44\x00" //cmp ecx,0044F48F
						"\x74\x5B"
						"\x81\xF9\xF0\xE9\x44\x00" //cmp ecx,0044E9F0
						"\x74\x61"
						"\x81\xF9\x4F\xED\x41\x00" //cmp ecx, 0041ED4F
						"\x74\x67"
						"\x81\xF9\x39\xFE\x41\x00" //cmp ecx, 0041FE39
						"\x74\x6D"
						"\x8D\x1C\x08" //lea ebx,[eax+ecx]
						"\x0F\xB6\x1B" //movzx ebx,byte ptr [ebx]
						, 0x46);
					hookFunc(hProc, 0xE9, BaseZF_07 + 0x80BE2, (DWORD)pbCheck + 0x46); //return to func

					writeMem(hProc, (DWORD)pbCheck + 0x4B, (byte*)"\x8D\x98", 2); //lea ebx, [eax + 00000000]
					write(hProc, (DWORD)pbCheck + 0x4D, pAllocZF, 4); //Addr pAllocZF
					writeMem(hProc, (DWORD)pbCheck + 0x51, (byte*)"\x0F\xB6\x1B", 3); //movzx ebx,byte ptr [ebx]
					hookFunc(hProc, 0xE9, BaseZF_07 + 0x80BE2, (DWORD)pbCheck + 0x54); //return to func

					writeMem(hProc, (DWORD)pbCheck + 0x59, (byte*)"\x8D\x98", 2); //lea ebx, [eax + 00000000]
					write(hProc, (DWORD)pbCheck + 0x5B, pAllocGame, 4); //Addr pAllocGame
					writeMem(hProc, (DWORD)pbCheck + 0x5F, (byte*)"\x0F\xB6\x1B", 3); //movzx ebx,byte ptr [ebx]
					hookFunc(hProc, 0xE9, BaseZF_07 + 0x80BE2, (DWORD)pbCheck + 0x62); //return to func

					//cmp ecx,004E1C6D
					writeMem(hProc, (DWORD)pbCheck + 0x67, (byte*)"\x8D\x98", 2);
					write(hProc, (DWORD)pbCheck + 0x69, pAllocGame + 0xE0C6D, 4);
					writeMem(hProc, (DWORD)pbCheck + 0x6D, (byte*)"\x0F\xB6\x1B", 3);
					hookFunc(hProc, 0xE9, BaseZF_07 + 0x80BE2, (DWORD)pbCheck + 0x70); //return to func

					//cmp ecx,004E1C15
					writeMem(hProc, (DWORD)pbCheck + 0x75, (byte*)"\x8D\x98", 2);
					write(hProc, (DWORD)pbCheck + 0x77, pAllocGame + 0xE0C15, 4);
					writeMem(hProc, (DWORD)pbCheck + 0x7B, (byte*)"\x0F\xB6\x1B", 3);
					hookFunc(hProc, 0xE9, BaseZF_07 + 0x80BE2, (DWORD)pbCheck + 0x7E); //return to func

					//cmp ecx,0044F48F
					writeMem(hProc, (DWORD)pbCheck + 0x83, (byte*)"\x8D\x98", 2);
					write(hProc, (DWORD)pbCheck + 0x85, pAllocGame + 0x4E48F, 4);
					writeMem(hProc, (DWORD)pbCheck + 0x89, (byte*)"\x0F\xB6\x1B", 3);
					hookFunc(hProc, 0xE9, BaseZF_07 + 0x80BE2, (DWORD)pbCheck + 0x8C); //return to func

					//cmp ecx,0044E9F0
					writeMem(hProc, (DWORD)pbCheck + 0x91, (byte*)"\x8D\x98", 2);
					write(hProc, (DWORD)pbCheck + 0x93, pAllocGame + 0x4D9F0, 4);
					writeMem(hProc, (DWORD)pbCheck + 0x97, (byte*)"\x0F\xB6\x1B", 3);
					hookFunc(hProc, 0xE9, BaseZF_07 + 0x80BE2, (DWORD)pbCheck + 0x9A); //return to func

					//cmp ecx, 0041ED4F
					writeMem(hProc, (DWORD)pbCheck + 0x9F, (byte*)"\x8D\x98", 2);
					write(hProc, (DWORD)pbCheck + 0xA1, pAllocGame + 0x1DD4F, 4);
					writeMem(hProc, (DWORD)pbCheck + 0xA5, (byte*)"\x0F\xB6\x1B", 3);
					hookFunc(hProc, 0xE9, BaseZF_07 + 0x80BE2, (DWORD)pbCheck + 0xA8); //return to func

					//cmp ecx, 0041FE39
					writeMem(hProc, (DWORD)pbCheck + 0xAD, (byte*)"\x8D\x98", 2);
					write(hProc, (DWORD)pbCheck + 0xAF, pAllocGame + 0x1EE39, 4);
					writeMem(hProc, (DWORD)pbCheck + 0xB3, (byte*)"\x0F\xB6\x1B", 3);
					hookFunc(hProc, 0xE9, BaseZF_07 + 0x80BE2, (DWORD)pbCheck + 0xB6); //return to func

					//------------------------ Restaure ------------------------//

					//dm_SelectRange
					copy_paste(hProc, codeGame + 0x57B5, (void*)0x004067B5, 0x5);
					//dm_SendRangeDamage
					copy_paste(hProc, codeGame + 0x683A, (void*)0x0040783A, 0x267);
					//dm_SendTransDamage
					copy_paste(hProc, codeGame + 0x63B9, (void*)0x004073B9, 0x3A6);
					//GameSave
					copy_paste(hProc, codeGame + 0x1DE283, (void*)0x005DF283, 0xDE);
					//PlayAttack
					copy_paste(hProc, codeGame + 0x1DD4A, (void*)0x0041ED4A, 0x1117);
					//SetItemToChar
					copy_paste(hProc, codeGame + 0x9FC7A, (void*)0x004A0C7A, 0x44A4);
					//sinGetLife / sinSetLife / sinSetMana / sinSetStm
					copy_paste(hProc, codeGame + 0xE0BDF, (void*)0x004E1BDF, 0xCF);
					//CoreMagico
					copy_paste(hProc, codeGame + 0x89434, (void*)0x0048A434, 0x6);
					copy_paste(hProc, codeGame + 0x88C1D, (void*)0x00489C1D, 0x6);

					//------------------------ Damage ------------------------//

					void* pSleep = GetProcAddress(GetModuleHandle("Kernel32.dll"), "Sleep");
					pOndaNegra = VirtualAllocEx(hProc, 0, 0x90, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

					hookFunc(hProc, 0xE9, (DWORD)pOndaNegra, 0x0042C5EC); //call to pOndaNegra

					writeMem(hProc, (DWORD)pOndaNegra, (byte*)"\x80\x3D", 0x2);
					write(hProc, (DWORD)pOndaNegra + 0x2, (DWORD)pOndaNegra + 0x78, 4);
					write(hProc, (DWORD)pOndaNegra + 0x6, 0, 1);
					HookFunc_2(hProc, 0x0042CAF3, (DWORD)pOndaNegra + 0x7);
					writeMem(hProc, (DWORD)pOndaNegra + 0xD, (byte*)"\x68\x00\x03\x00\x00", 0x5); //push 300
					hookFunc(hProc, 0xE8, (DWORD)pSleep, (DWORD)pOndaNegra + 0x12); //call sleep
					writeMem(hProc, (DWORD)pOndaNegra + 0x17, (byte*)"\x6A\x00"
						"\x6A\x46\xA1\xC0\xE6\xAF\x00\xFF\xB0\xF0\x01\x00\x00\xFF"
						"\xB0\xEC\x01\x00\x00\xFF\xB0\xE8\x01\x00\x00", 0x1B);
					hookFunc(hProc, 0xE8, 0x0040680D, (DWORD)pOndaNegra + 0x32); //dm_SendRange

					writeMem(hProc, (DWORD)pOndaNegra + 0x37, (byte*)"\x68\x1b\x01"
						"\x00\x00\x6A\x00\x6A\x00\x6A\x00\x6A\x00\x6A\x00\xFF\xB0\xF0\x01\x00\x00"
						"\xFF\xB0\xEC\x01\x00\x00\xFF\xB0\xE8\x01\x00\x00", 0x21);
					hookFunc(hProc, 0xE8, 0x0040783A, (DWORD)pOndaNegra + 0x58); //dm_SendRangeDamage

					writeMem(hProc, (DWORD)pOndaNegra + 0x5D, (byte*)"\x83\xC4\x38\x80\x3d", 0x5);
					write(hProc, (DWORD)pOndaNegra + 0x62, (DWORD)pOndaNegra + 0x78, 4);
					writeMem(hProc, (DWORD)pOndaNegra + 0x66, (byte*)"\x01"
						"\x7c\x9c\xc7\x05", 0x5);
					write(hProc, (DWORD)pOndaNegra + 0x6B, (DWORD)pOndaNegra + 0x78, 4);
					writeMem(hProc, (DWORD)pOndaNegra + 0x7F, (byte*)"\x00\x00\x00\x00", 4);
					hookFunc(hProc, 0xE9, 0x0042CAF3, (DWORD)pOndaNegra + 0x73);

					//------------------------ Hook Packets ------------------------//

					pLogs = VirtualAllocEx(hProc, 0, 0x50, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

					hookFunc(hProc, 0xE9, (DWORD)pLogs, 0x45F655);
					hookFunc(hProc, 0xE8, 0x006280C1, (DWORD)pLogs);  //call 006280C1
					writeMem(hProc, (DWORD)pLogs + 0x5, (byte*)"\x8B\x45\x08" //mov eax,[ebp+08]
						"\x8B\x50\x04" //mov edx,[eax+04]
						"\x89\x15", 8);
					write(hProc, (DWORD)pLogs + 0xD, (DWORD)pLogs + 0x28, 4); //mov [pLogs + 0x28],edx
					writeMem(hProc, (DWORD)pLogs + 0x11, (byte*)"\x8b\x55\x54\x89\x15", 5); //mov edx,[ebp+54]
					write(hProc, (DWORD)pLogs + 0x16, (DWORD)pLogs + 0x2C, 4); //mov [pLogs + 0x2c],edx
					writeMem(hProc, (DWORD)pLogs + 0x1A, (byte*)"\x8b\x50\x0c\x89\x15", 5);
					write(hProc, (DWORD)pLogs + 0x1F, (DWORD)pLogs + 0x30, 4); //mov [pLogs + 0x30],edx
					hookFunc(hProc, 0xE9, 0x0045F65A, (DWORD)pLogs + 0x23); //return

					//------------------------ disable packets ------------------------//

					writeMem(hProc, 0x0044E9F0, (byte*)"\xC3", 1); //CheckEnergyGraphError | Trava
					writeMem(hProc, 0x004801E7, (byte*)"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 20); //Drop Item | Core Magico


					ResumeThread(hThread);

					bPatch = true, bPatchActive = true;
				}
				else {
					sGameStatus = "Game Status -> Error ao realizar varredura na sessao .code!";
					bConsoleUpdate = true, bPatch = true;
				}
			}
		}
	}
}