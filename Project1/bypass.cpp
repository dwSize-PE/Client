#include "bypass.h"

using namespace std;

extern HANDLE hProc, hThread;
extern string sGameStatus;
extern bool bConsoleUpdate, bActive;

bool bPatch, bPatchActive, bCopyGame;
void* pSkill, * pLogs, * pfield, * pMob;

void memory() {
	int hooksGame, hooksGame2, codeGame;

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
				int BaseZF2_0A = aobScan(hProc, 0, 0x124000);
				int refBase_07 = 0x80C84;

				//Se encontrar..
				if (BaseZF_07 > 0 && BaseZF2_0A > 0 && readMem(hProc, BaseZF_07 + refBase_07, 3) == 0x081C8D) {
					sGameStatus = "Game Status -> Aguardando resposta de ZForce.dll..";
					bConsoleUpdate = true;

					//Suspende a thread para fazer as alterações afim de prevenir detecções antecipadas.
					SuspendThread(hThread);

					//copia todos os bytes da sessão ZForce
					int pAllocZF = copy_paste(hProc, BaseZF_07, 0, 0x1315F4);
					int pAllocZF2 = copy_paste(hProc, BaseZF2_0A + 0x100, 0, 0x116EFE);

					//copia todos os bytes da sessão Game.exe
					int pAllocGame = copy_paste(hProc, 0x00401000, 0, 0x247FFF);

					//espaço alocado dinâmico para pôr os bytes da sessão ZForce
					void* pbCheck = VirtualAllocEx(hProc, 0, 0xBC, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

					hookFunc(hProc, 0xE9, (DWORD)pbCheck, BaseZF_07 + refBase_07, (byte*)"\x90", 1); //Hook para o endereço alocado

					writeMem(hProc, (DWORD)pbCheck, (byte*)"\x81\xFE\xF4\x15\x13\x00" //cmp esi, 001315D0
						"\x74\x4B"	//je ..
						"\x81\xFE\xFF\x7F\x24\x00" //cmp esi,00247FFF
						"\x74\x51" //je ..
						"\x81\xF9\x6D\x1C\x4E\x00" //cmp ecx,004E1C6D
						"\x74\x57"
						"\x81\xF9\x15\x1C\x4E\x00" //cmp ecx,004E1C15
						"\x74\x5D"
						"\x81\xF9\x8F\xF4\x44\x00" //cmp ecx,0044F48F
						"\x74\x63"
						"\x81\xF9\xF0\xE9\x44\x00" //cmp ecx,0044E9F0
						"\x74\x69"
						"\x81\xF9\x4F\xED\x41\x00" //cmp ecx, 0041ED4F
						"\x74\x6F"
						"\x81\xF9\x39\xFE\x41\x00" //cmp ecx, 0041FE39
						"\x74\x75"
						"\x81\xFE\xFE\x6E\x11\x00" //cmp esi,00116EFE
						"\x74\x7B" //je ..
						"\x8D\x1C\x08" //lea ebx,[eax+ecx]
						"\x0F\xB6\x1B" //movzx ebx,byte ptr [ebx]
						, 0x4E);
					hookFunc(hProc, 0xE9, BaseZF_07 + refBase_07 + 0x6, (DWORD)pbCheck + 0x4E); //return to func

					//cmp esi, 1315D0
					writeMem(hProc, (DWORD)pbCheck + 0x53, (byte*)"\x8D\x98", 2); //lea ebx, [eax + 00000000]
					write(hProc, (DWORD)pbCheck + 0x55, pAllocZF, 4); //Addr pAllocZF
					writeMem(hProc, (DWORD)pbCheck + 0x59, (byte*)"\x0F\xB6\x1B", 3); //movzx ebx,byte ptr [ebx]
					hookFunc(hProc, 0xE9, BaseZF_07 + refBase_07 + 0x6, (DWORD)pbCheck + 0x5C); //return to func

					//cmp esi, 247FFF
					writeMem(hProc, (DWORD)pbCheck + 0x61, (byte*)"\x8D\x98", 2); //lea ebx, [eax + 00000000]
					write(hProc, (DWORD)pbCheck + 0x63, pAllocGame, 4); //Addr pAllocGame
					writeMem(hProc, (DWORD)pbCheck + 0x67, (byte*)"\x0F\xB6\x1B", 3); //movzx ebx,byte ptr [ebx]
					hookFunc(hProc, 0xE9, BaseZF_07 + refBase_07 + 0x6, (DWORD)pbCheck + 0x6A); //return to func

					//cmp ecx,004E1C6D
					writeMem(hProc, (DWORD)pbCheck + 0x6F, (byte*)"\x8D\x98", 2);
					write(hProc, (DWORD)pbCheck + 0x71, pAllocGame + 0xE0C6D, 4);
					writeMem(hProc, (DWORD)pbCheck + 0x75, (byte*)"\x0F\xB6\x1B", 3);
					hookFunc(hProc, 0xE9, BaseZF_07 + refBase_07 + 0x6, (DWORD)pbCheck + 0x78); //return to func

					//cmp ecx,004E1C15
					writeMem(hProc, (DWORD)pbCheck + 0x7D, (byte*)"\x8D\x98", 2);
					write(hProc, (DWORD)pbCheck + 0x7F, pAllocGame + 0xE0C15, 4);
					writeMem(hProc, (DWORD)pbCheck + 0x83, (byte*)"\x0F\xB6\x1B", 3);
					hookFunc(hProc, 0xE9, BaseZF_07 + refBase_07 + 0x6, (DWORD)pbCheck + 0x86); //return to func

					//cmp ecx,0044F48F
					writeMem(hProc, (DWORD)pbCheck + 0x8B, (byte*)"\x8D\x98", 2);
					write(hProc, (DWORD)pbCheck + 0x8D, pAllocGame + 0x4E48F, 4);
					writeMem(hProc, (DWORD)pbCheck + 0x91, (byte*)"\x0F\xB6\x1B", 3);
					hookFunc(hProc, 0xE9, BaseZF_07 + refBase_07 + 0x6, (DWORD)pbCheck + 0x94); //return to func

					//cmp ecx,0044E9F0
					writeMem(hProc, (DWORD)pbCheck + 0x99, (byte*)"\x8D\x98", 2);
					write(hProc, (DWORD)pbCheck + 0x9B, pAllocGame + 0x4D9F0, 4);
					writeMem(hProc, (DWORD)pbCheck + 0x9F, (byte*)"\x0F\xB6\x1B", 3);
					hookFunc(hProc, 0xE9, BaseZF_07 + refBase_07 + 0x6, (DWORD)pbCheck + 0xA2); //return to func

					//cmp ecx, 0041ED4F
					writeMem(hProc, (DWORD)pbCheck + 0xA7, (byte*)"\x8D\x98", 2);
					write(hProc, (DWORD)pbCheck + 0xA9, pAllocGame + 0x1DD4F, 4);
					writeMem(hProc, (DWORD)pbCheck + 0xAD, (byte*)"\x0F\xB6\x1B", 3);
					hookFunc(hProc, 0xE9, BaseZF_07 + refBase_07 + 0x6, (DWORD)pbCheck + 0xB0); //return to func

					//cmp ecx, 0041FE39
					writeMem(hProc, (DWORD)pbCheck + 0xB5, (byte*)"\x8D\x98", 2);
					write(hProc, (DWORD)pbCheck + 0xB7, pAllocGame + 0x1EE39, 4);
					writeMem(hProc, (DWORD)pbCheck + 0xBB, (byte*)"\x0F\xB6\x1B", 3);
					hookFunc(hProc, 0xE9, BaseZF_07 + refBase_07 + 0x6, (DWORD)pbCheck + 0xBE); //return to func

					//cmp esi, 0x116EFE
					writeMem(hProc, (DWORD)pbCheck + 0xC3, (byte*)"\x8D\x98", 2); //lea ebx, [eax + 00000000]
					write(hProc, (DWORD)pbCheck + 0xC5, pAllocZF2, 4); //Addr pAllocZF2
					writeMem(hProc, (DWORD)pbCheck + 0xC9, (byte*)"\x0F\xB6\x1B", 3); //movzx ebx,byte ptr [ebx]
					hookFunc(hProc, 0xE9, BaseZF_07 + refBase_07 + 0x6, (DWORD)pbCheck + 0xCC); //return to func

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
					//WarpField2
					copy_paste(hProc, codeGame + 0x35BBB, (void*)0x00436BBB, 6);
					//send2
					copy_paste(hProc, codeGame + 0x5E5AA, (void*)0x0045F5AA, 6);

					//------------------------ Damage ------------------------//

					pSkill = VirtualAllocEx(hProc, 0, 0x90, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

					//hookFunc(hProc, 0xE9, (DWORD)pSkill, 0x0042C5EC); //Força Divina | Shaman
					hookFunc(hProc, 0xE9, (DWORD)pSkill, 0x0042BF16); //Manha de Oracao | Shaman
					hookFunc(hProc, 0xE9, (DWORD)pSkill, 0x0042A7F9, (byte*)"\x90\x90", 2); //Meteoro | Mago
					hookFunc(hProc, 0xE9, (DWORD)pSkill, 0x0042B48C, (byte*)"\x90\x90", 2); //Impacto | Sacer

					writeMem(hProc, (DWORD)pSkill, (byte*)"\x80\x3D", 0x2);
					write(hProc, (DWORD)pSkill + 0x2, (DWORD)pSkill + 0x97, 4);
					write(hProc, (DWORD)pSkill + 0x6, 0, 1);
					writeMem(hProc, (DWORD)pSkill + 0x7, (byte*)"\x74\x6D", 2);

					writeMem(hProc, (DWORD)pSkill + 0x9, (byte*)
						"\x8B\x1D\x0C\xE6\xAF\x00" //mov ebx,[00AFE60C] - lpCurPlayer
						"\x8B\x83\x9C\x02\x00\x00" //mov eax,[ebx+0000029C] - chrAttackTarget
						"\x56" //push esi
						"\x68\xC8\x00\x00\x00" //push 200
						"\xFF\xB0\xF0\x01\x00\x00" //push [eax+000001F0] //target coordenate z
						"\xFF\xB0\xEC\x01\x00\x00" //push [eax+000001EC] //target coordenate y
						"\xFF\xB0\xE8\x01\x00\x00", 0x24); //push [eax+000001E8] //target coordenate x
					hookFunc(hProc, 0xE8, 0x0040680D, (DWORD)pSkill + 0x2D); //dm_SelectRange

					writeMem(hProc, (DWORD)pSkill + 0x32, (byte*)
						"\xA1\xB4\xB9\x6A\x00" //mov eax,[006AB9B4]
						"\xFF\xB3\xAC\x02\x00\x00" //push [ebx+000002AC] //dwSkillCode
						"\x69\xC0\x14\x03\x00\x00" //imul eax,eax,00000314
						"\x0F\xBF\x90\x26\x29\x3B\x03" //movsx edx,word ptr [eax+033B2926]
						"\x0F\xBF\x80\x24\x29\x3B\x03" //movsx eax,word ptr [eax+033B2924]
						"\x8B\x8B\x9C\x02\x00\x00" //mov ecx,[ebx+0000029C] //chrAttackTarget
						"\x56" //push esi
						"\x56" //push esi
						"\x52" //push edx
						"\x50" //push eax
						"\x56" //push esi
						"\xFF\xB1\xF0\x01\x00\x00" //push [ecx+000001F0] //target coordenate z
						"\xFF\xB1\xEC\x01\x00\x00" //push [ecx+000001EC] //target coordenate y
						"\xFF\xB1\xE8\x01\x00\x00", 0x3C); //push [ecx+000001E8] //target coordenate x
					hookFunc(hProc, 0xE8, 0x0040783A, (DWORD)pSkill + 0x6E); //call dm_SendRangeDamage
					writeMem(hProc, (DWORD)pSkill + 0x73, (byte*)"\x83\xC4\x38", 3); //add esp, 38
					writeMem(hProc, (DWORD)pSkill + 0x76, (byte*)"\x80\x05", 2);
					write(hProc, (DWORD)pSkill + 0x78, (DWORD)pSkill + 0x98, 4);
					writeMem(hProc, (DWORD)pSkill + 0x7C, (byte*)"\x01\x80\x3D", 3);
					write(hProc, (DWORD)pSkill + 0x7F, (DWORD)pSkill + 0x98, 4);
					write(hProc, (DWORD)pSkill + 0x83, 1, 1);
					HookFunc_2(hProc, (DWORD)pSkill, (DWORD)pSkill + 0x84);
					writeMem(hProc, (DWORD)pSkill + 0x8A, (byte*)"\xc6\x05", 2);
					write(hProc, (DWORD)pSkill + 0x8C, (DWORD)pSkill + 0x98, 4);
					hookFunc(hProc, 0xE9, 0x0042CAF3, (DWORD)pSkill + 0x91); //return to func

					//------------------------ Teleporte ------------------------//
					void* ptelep = VirtualAllocEx(hProc, NULL, 0x50, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
					pfield = VirtualAllocEx(hProc, NULL, 0x4, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

					hookFunc(hProc, 0xE9, (DWORD)ptelep, 0x00410B0C);

					writeMem(hProc, (DWORD)ptelep, (byte*)"\x83\x3d", 2);
					write(hProc, (DWORD)ptelep + 0x2, (DWORD)pfield, 4); //cmp [field], 0
					writeMem(hProc, (DWORD)ptelep + 0x7, (byte*)"\x74\x16", 2); //je 
					writeMem(hProc, (DWORD)ptelep + 0x9, (byte*)"\xff\x35", 2);
					write(hProc, (DWORD)ptelep + 0xB, (DWORD)pfield, 4); //push [field]
					hookFunc(hProc, 0xE8, 0x00436BB5, (DWORD)ptelep + 0xF); //call WarpField2
					writeMem(hProc, (DWORD)ptelep + 0x14, (byte*)"\x59", 10); //pop ecx
					writeMem(hProc, (DWORD)ptelep + 0x15, (byte*)"\xc7\x05", 2);
					write(hProc, (DWORD)ptelep + 0x17, (DWORD)pfield, 4); //mov [field], 0
					write(hProc, (DWORD)ptelep + 0x1B, 0, 4);

					//------------------------ dm_SendTransDamage ------------------------//
					void* pDamage = (void*)((DWORD)ptelep + 0x1F);
					pMob = VirtualAllocEx(hProc, NULL, 0x4, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

					writeMem(hProc, (DWORD)pDamage, (byte*)"\x83\x3d", 2);
					write(hProc, (DWORD)pDamage + 0x2, (DWORD)pMob, 4); //cmp [pMob]
					writeMem(hProc, (DWORD)pDamage + 0x7, (byte*)"\x74\x73", 2); //je
					writeMem(hProc, (DWORD)pDamage + 0x9, (byte*)"\xA1", 1);
					write(hProc, (DWORD)pDamage + 0xA, (DWORD)pMob, 4); //mov [pMob]
					writeMem(hProc, (DWORD)pDamage + 0xE, (byte*)
						"\x6a\x00"
						"\x68\xc8\x00\x00\x00"
						"\xff\xb0\xe0\x01\x00\x00"
						"\xff\xb0\xdc\x01\x00\x00"
						"\xff\xb0\xd8\x01\x00\x00", 0x19);
					hookFunc(hProc, 0xE8, 0x0040680D, (DWORD)pDamage + 0x27);

					writeMem(hProc, (DWORD)pDamage + 0x2C, (byte*)"\xA1", 1);
					write(hProc, (DWORD)pDamage + 0x2D, 0x006AB9B4, 4);
					writeMem(hProc, (DWORD)pDamage + 0x31, (byte*)
						"\x68\x4d\x01\x00\x00"
						"\x69\xc0\x14\x03\x00\x00"
						"\x0f\xbf\x90", 0xE);
					write(hProc, (DWORD)pDamage + 0x3F, 0x033B2926, 4);
					writeMem(hProc, (DWORD)pDamage + 0x43, (byte*)"\x0f\xbf\x80", 3);
					write(hProc, (DWORD)pDamage + 0x46, 0x033B2926 - 0x2, 4);
					writeMem(hProc, (DWORD)pDamage + 0x4A, (byte*)"\x8b\x0d", 2);
					write(hProc, (DWORD)pDamage + 0x4C, (DWORD)pMob, 4);
					writeMem(hProc, (DWORD)pDamage + 0x50, (byte*)
						"\x6a\x00"
						"\x6a\x00"
						"\x52"
						"\x50"
						"\x6a\x00"
						"\xff\xb0\xe0\x01\x00\x00"
						"\xff\xb0\xdc\x01\x00\x00"
						"\xff\xb0\xd8\x01\x00\x00", 0x1A);
					hookFunc(hProc, 0xE8, 0x0040783A, (DWORD)pDamage + 0x6A);
					writeMem(hProc, (DWORD)pDamage + 0x6F, (byte*)
						"\xc7\x05", 2);
					write(hProc, (DWORD)pDamage + 0x71, (DWORD)pMob, 4);
					writeMem(hProc, (DWORD)pDamage + 0x75, (byte*)
						"\x00\x00\x00\x00"
						"\x83\xc4\x38"
						"\x55"
						"\x8b\xec"
						"\x83\xec\x1c", 0xD);

					hookFunc(hProc, 0xE9, 0x00410B12, (DWORD)pDamage + 0x82);

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
					writeMem(hProc, 0x004801C4, (byte*)"\xEB\x45", 2); //Drop core
					writeMem(hProc, 0x004801C4 + 0x47, (byte*)"\x59\x90\x90\x90\x90", 5);
					//writeMem(hProc, BaseZF2_0A + 0x30910, (byte*)"\xc3", 1);
					//writeMem(hProc, 0x005DF2D9, (byte*)"\xe9\x84\x00\x00\x00", 5); //Game Unsave
					//writeMem(hProc, 0x0044D145, (byte*)"\xEB", 1); //CheckCharForm

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
