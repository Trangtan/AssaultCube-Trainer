// GH1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <ctime>
#include <Windows.h>
#include <string>

std::string gameName = "AssaultCube";
LPCSTR LGameWindow = "AssaultCube";
std::string gameStatus;

bool isGameAvail;
bool updateOnNextRun;

//Ammo VARS
bool AmmoStatus;
BYTE AmmoValue[] = { 0xA3, 0x1C, 0x0, 0x0 };
DWORD AmmoBaseAddress = { 0x50f4f4 };
DWORD AmmoOffsets[] = { 0x378, 0x14, 0x0 };

//Health VARS
bool HealthStatus;
BYTE HealthValue[] = { 0x39, 0x5, 0x0, 0x0 };
DWORD HealthBaseAddress = { 0x50f4f4 };
DWORD HealthOffsets[] = { 0xf8 };

DWORD FindDmaAddy(int PointerLevel, HANDLE hProcHandle, DWORD offsets[], DWORD BaseAddress);
void WriteToMemory(HANDLE hProcHandle);
int main()
{
	HWND hGameWindow = NULL;
	int timeSinceLastUpdate = clock();
	int gameAvailTMR = clock();
	int onePressTMR = clock();
	DWORD dwProcID = NULL;
	HANDLE hProcHandle = NULL;
	updateOnNextRun = true;
	std::string sAmmoStatus = "OFF";
	std::string sHealthStatus = "OFF";

	while (!GetAsyncKeyState(VK_INSERT)) {
		if (clock() - gameAvailTMR > 100) {
			gameAvailTMR = clock();
			isGameAvail = false;
			hGameWindow = FindWindow(NULL, LPCWSTR(LGameWindow));
			if (hGameWindow) {
				GetWindowThreadProcessId(hGameWindow, &dwProcID);
				if (dwProcID != 0) {
					hProcHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcID);
					if (hProcHandle == INVALID_HANDLE_VALUE || hProcHandle == NULL) {
						gameStatus = "Failed to open process for valid handle";
					}
					else {
						gameStatus = "AssaultCube Ready To Hack";
						isGameAvail = true;
					}
				}
				else {
					gameStatus = "Failed To Get ProcesId";
				}
			}
			else {
				gameStatus = "AssaultCube Not Found";
			}
			if (updateOnNextRun || clock() - timeSinceLastUpdate > 5000) {
				system("cls");
				std::cout << "-------------------------------------------------" << std::endl;
				std::cout << "        AssaultCube memory hacker      " << std::endl;
				std::cout << "-------------------------------------------------" << std::endl << std::endl;
				std::cout << "[F1] Unlimited ammo ->" << sAmmoStatus << "<-" << std::endl;
				std::cout << "[F2] Unlimited Health ->" << sHealthStatus << "<-" << std::endl;
				std::cout << "[Insert] Exit" << std::endl;
				updateOnNextRun = false;
				timeSinceLastUpdate = clock();
			}
			if (isGameAvail) {
				//Write to memory
				WriteToMemory(hProcHandle);
			}
		}
		if (clock() - onePressTMR > 400) {
			if (isGameAvail) {
				//ammo
				if (GetAsyncKeyState(VK_F1)) {
					onePressTMR = clock();
					AmmoStatus = !AmmoStatus;
					updateOnNextRun = true;
					if (AmmoStatus)sAmmoStatus = "ON";
					else sAmmoStatus = "OFF";

				}
				//health
				else if (GetAsyncKeyState(VK_F2)) {
					onePressTMR = clock();
					HealthStatus = !HealthStatus;
					updateOnNextRun = true;
					if (HealthStatus)sHealthStatus = "ON";
					else sHealthStatus = "OFF";
				}

			}
		}
	}
	CloseHandle(hProcHandle);
	CloseHandle(hGameWindow);
	return ERROR_SUCCESS;
}

DWORD FindDmaAddy(int PointerLevel, HANDLE hProcHandle, DWORD offsets[], DWORD BaseAddress) {
	DWORD pointer = BaseAddress;
	DWORD pTemp;
	DWORD pointerAddr;
	for (int c = 0; c < PointerLevel; c++) {
		if (c == 0) {
			ReadProcessMemory(hProcHandle, (LPCVOID)pointer, &pTemp, sizeof(pTemp), NULL);
		}
		pointerAddr = pTemp + offsets[c];
		ReadProcessMemory(hProcHandle, (LPCVOID)pointer, &pTemp, sizeof(pTemp), NULL);
	}
	return pointerAddr;
}

void WriteToMemory(HANDLE hProcHandle) {
	if (AmmoStatus) {
		//DWORD AmmoAddressToWrite = FindDmaAd
		DWORD AddressToWrite;
		if (AmmoStatus) {
			AddressToWrite = FindDmaAddy(3, hProcHandle, AmmoOffsets, AmmoBaseAddress);
			WriteProcessMemory(hProcHandle, (BYTE*)AddressToWrite, &AmmoValue, sizeof(AmmoValue), NULL);
		}
		if (HealthStatus) {
			AddressToWrite = FindDmaAddy(1, hProcHandle, HealthOffsets, HealthBaseAddress);
			WriteProcessMemory(hProcHandle, (BYTE*)AddressToWrite, &HealthValue, sizeof(HealthValue), NULL);
		}
	}
}

