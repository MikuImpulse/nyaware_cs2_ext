#pragma once

#include <cstdint>
#include <string>
#include <map>

class c_schema_dumper {
public:
	static inline std::map<std::string, std::map<std::string, uint32_t>> schema_offset{};

	bool dump(uintptr_t schema_system_interface);
};