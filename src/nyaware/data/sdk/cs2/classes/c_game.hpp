#pragma once

#include "data/sdk/schema_dumper.hpp"
#include "utils/memory.hpp"

struct C_CSGameRules {
	inline bool m_bBombPlanted() const {
		return mem.read<bool>(this_cast + c_schema_dumper::schema_offset["C_CSGameRules"]["m_bBombPlanted"]);
	}
};