#pragma once
#include "includes.hpp"

auto get_module(const char* modName, DWORD procId) {
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry)) {
			do {
				if (!strcmp(modEntry.szModule, modName)) {
					CloseHandle(hSnap);
					return modEntry;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
}

template<typename T> T RPM(SIZE_T address) {
	T buffer; ReadProcessMemory(g_process_handle, (void*)address, &buffer, sizeof(T), nullptr);
	return buffer;
}

template<typename T> void WPM(SIZE_T address, T buffer) {
	WriteProcessMemory(g_process_handle, (void*)address, &buffer, sizeof(T), nullptr);
}

uintptr_t find_pattern(MODULEENTRY32 module, const char* pattern, const char* mask) {
	SIZE_T bytesRead;
	DWORD oldprotect;
	MEMORY_BASIC_INFORMATION mbi = { 0 };

	auto scan = [](const char* pattern, const char* mask, char* begin, unsigned int size) -> char* {
		size_t patternLen = strlen(mask);
		for (unsigned int i = 0; i < size - patternLen; i++) {
			bool found = true;
			for (unsigned int j = 0; j < patternLen; j++) {
				if (mask[j] != '?' && pattern[j] != *(begin + i + j)) {
					found = false;
					break;
				}
			}
			if (found) { return (begin + i); }
		}
		return nullptr;
	};

	for (uintptr_t curr = (uintptr_t)module.modBaseAddr; curr < (uintptr_t)module.modBaseAddr + module.modBaseSize; curr += mbi.RegionSize) {
		if (!VirtualQueryEx(g_process_handle, (void*)curr, &mbi, sizeof(mbi))) continue;
		if (mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS) continue;

		char* buffer = new char[mbi.RegionSize];

		if (VirtualProtectEx(g_process_handle, mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &oldprotect)) {
			ReadProcessMemory(g_process_handle, mbi.BaseAddress, buffer, mbi.RegionSize, &bytesRead);
			VirtualProtectEx(g_process_handle, mbi.BaseAddress, mbi.RegionSize, oldprotect, &oldprotect);

			char* internalAddr = scan(pattern, mask, buffer, bytesRead);

			if (internalAddr != nullptr) { return curr + (uintptr_t)(internalAddr - buffer); }
		}
		delete[] buffer;
	}
	return 0x0;
}