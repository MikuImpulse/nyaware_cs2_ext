#include "memory.hpp"

#include <tlhelp32.h>
#include <atlconv.h>

#include <vector>

int c_memory_manager::get_processID(std::string process_name) {
	PROCESSENTRY32 process_info{};
	process_info.dwSize = sizeof(PROCESSENTRY32);

	HANDLE help_snapshot = CreateToolhelp32Snapshot(15, 0);
	if (help_snapshot != INVALID_HANDLE_VALUE) {
		Process32First(help_snapshot, &process_info);

		USES_CONVERSION;

		do {
			if (strcmp(W2A(process_info.szExeFile), process_name.c_str()) == 0) {
				CloseHandle(help_snapshot);
				return process_info.th32ProcessID;
			}
		} while (Process32Next(help_snapshot, &process_info));
	}
	
	CloseHandle(help_snapshot);
	return 0;
}

uintptr_t c_memory_manager::findDLL(int process_id, std::string dll_name) {
	MODULEENTRY32 module_entry;
	module_entry.dwSize = sizeof(MODULEENTRY32);

	HANDLE help_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id);
	if (help_snapshot != INVALID_HANDLE_VALUE) {
		Module32First(help_snapshot, &module_entry);

		USES_CONVERSION;

		do {
			if (strcmp(W2A(module_entry.szModule), dll_name.c_str()) == 0) {
				CloseHandle(help_snapshot);
				return (uintptr_t)module_entry.modBaseAddr;
			}
		} while (Module32Next(help_snapshot, &module_entry));
	}

	CloseHandle(help_snapshot);
	return 0;
}

uintptr_t c_memory_manager::find_pattern(uintptr_t module_base, size_t module_size, const char* signature) {
	auto parse_pattern = [](const char* sig) {
		std::vector<int> bytes;
		bytes.reserve(strlen(sig) / 2);

		for (const char* p = sig; *p; ) {
			if (*p == ' ') { ++p; continue; }

			if (*p == '?') {
				bytes.push_back(-1);
				++p;
				if (*p == '?') ++p;
				continue;
			}

			bytes.push_back(strtoul(p, (char**)&p, 16));
		}

		return bytes;
	};

	auto pattern = parse_pattern(signature);
	std::vector<uint8_t> buffer(module_size);

	if (!this->read(module_base, buffer.data(), module_size)) return 0;

	const size_t pattern_size = pattern.size();

	for (size_t i = 0; i + pattern_size <= module_size; ++i) {
		size_t j = 0;
		for (; j < pattern_size; ++j)
			if (pattern[j] != -1 && buffer[i + j] != pattern[j])
				break;

		if (j == pattern_size)
			return i;
	}

	return 0;
}

uintptr_t c_memory_manager::resolve_pattern(uintptr_t module_base, size_t module_size, const char* signature) {
	uintptr_t rva = find_pattern(module_base, module_size, signature);
	if (rva) {
		uintptr_t instr = module_base + rva;
		int32_t rel = this->read<int32_t>(instr + 3);

		return (uintptr_t)(instr + 7 + rel);
	}

	return 0;
}

bool c_memory_manager::init(int process_id) {
	HMODULE ntdll_handle = GetModuleHandleA("ntdll.dll");
	if (ntdll_handle) {
		NtReadVirtualMemory = reinterpret_cast<decltype(NtReadVirtualMemory)>(GetProcAddress(ntdll_handle, "NtReadVirtualMemory"));
		NtWriteVirtualMemory = reinterpret_cast<decltype(NtWriteVirtualMemory)>(GetProcAddress(ntdll_handle, "NtWriteVirtualMemory"));

		this->processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, process_id);
		if (processHandle)
			return true;
	}

	return false;
}