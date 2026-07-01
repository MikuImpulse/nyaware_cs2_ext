#pragma once

#include <cstdint>
#include <string>
#include <windows.h>

#define this_cast reinterpret_cast<uintptr_t>(this)

inline NTSTATUS (NTAPI* NtReadVirtualMemory)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, SIZE_T NumberOfBytesToRead, PSIZE_T NumberOfBytesRead);
inline NTSTATUS (NTAPI* NtWriteVirtualMemory)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, SIZE_T NumberOfBytesToWrite, PSIZE_T NumberOfBytesWritten);

class c_memory_manager {
private:
	void* processHandle = nullptr;
public:
	int get_processID(std::string process_name);
	uintptr_t findDLL(int process_id, std::string dll_name);

	template <typename T>
	inline T read(uintptr_t address) {
		T buffer{};
		SIZE_T bytesRead = 0;

		NTSTATUS status = NtReadVirtualMemory(this->processHandle, (void*)address, &buffer, sizeof(T), nullptr);

		if (status == 0)
			return buffer;

		return {};
	}

	inline bool read(uintptr_t address, void* buffer, int size) {
		NTSTATUS status = NtReadVirtualMemory(this->processHandle, (void*)address, buffer, size, nullptr);
		return (status == 0);
	}

	inline std::string read_str(uintptr_t address) {
		char buffer[MAX_PATH]{};

		NTSTATUS status = NtReadVirtualMemory(this->processHandle, (void*)address, buffer, MAX_PATH, nullptr);

		if (status == 0)
			return std::string(buffer);

		return {};
	}

	template <typename T>
	inline bool write(uintptr_t address, T value) {
		NTSTATUS status = NtWriteVirtualMemory(this->processHandle, (void*)address, &value, sizeof(T), nullptr);

		return (status == 0);
	}

	uintptr_t find_pattern(uintptr_t module_base, size_t module_size, const char* signature);
	uintptr_t resolve_pattern(uintptr_t module_base, size_t module_size, const char* signature);

	bool init(int process_id);
};

inline c_memory_manager mem{};

struct dll_t {
	uintptr_t base{};
	size_t size{};

	inline bool isValid() {
		if (!base) return false;

		char magic[2] = { 0 };
		return mem.read(base, magic, sizeof(magic)) && memcmp(magic, "MZ", 2) == 0 && size > 0;
	}

	inline uintptr_t get_absoluteAddress(uintptr_t offset) {
		return this->isValid() ? base + offset : 0;
	}

	dll_t() = default;
	explicit dll_t(uintptr_t dll_base) : base(dll_base) {
		auto dos = mem.read<IMAGE_DOS_HEADER>(base);
		auto nt = mem.read<IMAGE_NT_HEADERS>(base + dos.e_lfanew);

		size = nt.OptionalHeader.SizeOfImage;
	};
};